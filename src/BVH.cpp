#include "BVH.h"
#include "Shape.h"

#include <algorithm>
#include <cassert>

namespace {

using namespace pt;

struct SplitBin {
    size_t numShapes = 0;
    BoundingBox bounds = BoundingBox(Vec3(inf<float>), Vec3(-inf<float>));
};

} // namespace


namespace pt {

BVH::BVH(const std::vector<const Shape*>& shapes, uint32_t maxShapesPerLeaf)
    : maxShapesPerLeaf_(maxShapesPerLeaf)
{
    assert(shapes.size() <= std::numeric_limits<uint32_t>::max());

    std::vector<ShapeInfo> shapeInfos;
    shapeInfos.reserve(shapes.size());
    for (size_t index = 0; index < shapes.size(); index++) {
        BoundingBox bounds = shapes[index]->getWorldBounds();
        shapeInfos.push_back({
            static_cast<uint32_t>(index),
            bounds,
            bounds.getCenter()
        });
    }

    size_t maxNumNodes = 2 * shapes.size() - 1;
    assert(maxNumNodes <= std::numeric_limits<uint32_t>::max());
    std::vector<BuildNode> buildNodes;
    buildNodes.reserve(maxNumNodes);
    uint32_t rootBuildNodeIndex = buildInternal(buildNodes, shapes,
        shapeInfos, 0, static_cast<uint32_t>(shapeInfos.size()));

    linearNodes_.reserve(buildNodes.size());
    rootNodeIndex_ = flattenTree(rootBuildNodeIndex, buildNodes);
}

RayHit BVH::intersect(Ray ray) const {
    Vec3 rayInvDirection = Vec3(1.0f) / ray.direction;
    bool raySign[3] = { ray.direction.x < 0.0f, ray.direction.y < 0.0f, ray.direction.z < 0.0f };

    constexpr uint32_t stackSize = 128; // Should be enough for moderately balanced trees
    uint32_t traversalStack[stackSize];
    uint32_t stackOffset = 0;
    uint32_t currentNodeIndex = rootNodeIndex_;

    RayHit closestHit = rayMiss;
    while (true) {
        const LinearNode& node = linearNodes_[currentNodeIndex];
        if (!testIntersection(ray, rayInvDirection, node.bounds)) {
            if (stackOffset == 0) {
                break;
            }
            currentNodeIndex = traversalStack[--stackOffset];
            assert(stackOffset < stackSize);
            continue;
        }

        if (!node.isLeaf()) {
            if (raySign[node.splitAxis]) {
                assert(stackOffset < stackSize);
                traversalStack[stackOffset++] = currentNodeIndex + 1; // First child is always the next index
                currentNodeIndex = node.secondChildOffset;
            }
            else {
                assert(stackOffset < stackSize);
                traversalStack[stackOffset++] = node.secondChildOffset;
                currentNodeIndex += 1; // First child is always the next index
            }
            continue;
        }

        for (uint32_t i = 0; i < node.numShapes; i++) {
            const Shape* shape = orderedShapes_[node.firstShapeIndex + i];
            RayHit hit = shape->intersect(ray);
            if (hit.t >= 0.0f && (closestHit.t < 0.0f || hit.t < closestHit.t)) {
                closestHit = hit;
                ray.tmax = hit.t;
            }
        }

        if (stackOffset == 0) {
            break;
        }
        currentNodeIndex = traversalStack[--stackOffset];
        assert(stackOffset < stackSize);
    }

    return closestHit;
}

void BVH::traverse(const TraversalCallback& callback) const {
    constexpr uint32_t stackSize = 128; // Should be enough for moderately balanced trees
    uint32_t traversalStack[stackSize];
    uint32_t stackOffset = 0;
    uint32_t currentNodeIndex = rootNodeIndex_;

    while (true) {
        const LinearNode& node = linearNodes_[currentNodeIndex];
        if (!callback(node)) {
            break;
        }

        if (!node.isLeaf()) {
            assert(stackOffset < stackSize);
            traversalStack[stackOffset++] = node.secondChildOffset;
            currentNodeIndex += 1; // First child is always the next index
            continue;
        }

        if (stackOffset == 0) {
            break;
        }
        currentNodeIndex = traversalStack[--stackOffset];
        assert(stackOffset < stackSize);
    }
}

uint32_t BVH::buildInternal(std::vector<BuildNode>& nodes, const std::vector<const Shape*>& shapes,
        std::vector<ShapeInfo>& shapeInfos, uint32_t left, uint32_t right) {
    uint32_t nodeIndex = static_cast<uint32_t>(nodes.size());
    BuildNode& node = nodes.emplace_back();

    // Used later when checking if a node is a leaf (axis = uint8_max) or not
    node.splitAxis = std::numeric_limits<uint8_t>::max();

    // The bounding box is the union of all the node's containing shape's bounding boxes
    node.bounds = shapeInfos[left].bounds;
    for (size_t i = left + 1; i < right; i++) {
        node.bounds.min = min(node.bounds.min, shapeInfos[i].bounds.min);
        node.bounds.max = max(node.bounds.max, shapeInfos[i].bounds.max);
    }

    size_t numShapes = right - left;
    if (numShapes == 1) {
        node.firstShapeIndex = static_cast<uint32_t>(orderedShapes_.size());
        node.numShapes = static_cast<uint32_t>(numShapes);
        for (size_t i = left; i < right; i++) {
            orderedShapes_.push_back(shapes[shapeInfos[i].shapeIndex]);
        }
        return nodeIndex;
    }

    BoundingBox centroidBounds(Vec3(inf<float>), Vec3(-inf<float>));
    for (size_t i = left; i < right; i++) {
        centroidBounds.min = min(centroidBounds.min, shapeInfos[i].centroid);
        centroidBounds.max = max(centroidBounds.max, shapeInfos[i].centroid);
    }
    uint32_t splitDimension = maxDimension(centroidBounds.getExtents());

    // Special case where the centroids of multiple shapes are stacked over eachother
    if (abs(centroidBounds.min[splitDimension] - centroidBounds.max[splitDimension]) < 1e-6f) {
        node.firstShapeIndex = static_cast<uint32_t>(orderedShapes_.size());
        node.numShapes = static_cast<uint32_t>(numShapes);
        for (size_t i = left; i < right; i++) {
            orderedShapes_.push_back(shapes[shapeInfos[i].shapeIndex]);
        }
        return nodeIndex;
    }

    uint32_t middle;
    if (numShapes <= 2) {
        // Split with equal counts
        middle = static_cast<uint32_t>((left + right) / 2);
        std::nth_element(shapeInfos.begin() + left,
            shapeInfos.begin() + middle, shapeInfos.begin() + right,
            [&](const ShapeInfo& a, const ShapeInfo& b) {
                return a.centroid[splitDimension] < b.centroid[splitDimension];
            });
    }
    else {
        // Split based on minimum SAH
        constexpr uint32_t numBins = 16;
        SplitBin splitBins[numBins];

        float centroidBoundsWidth = centroidBounds.max[splitDimension] - centroidBounds.min[splitDimension];
        float k1 = numBins * (1.0f - 1e-6f) / centroidBoundsWidth;
        float k0 = centroidBounds.min[splitDimension];
        for (uint32_t i = left; i < right; i++) {
            uint32_t binIndex = static_cast<uint32_t>(k1 * (shapeInfos[i].centroid[splitDimension] - k0));
            splitBins[binIndex].numShapes++;
            splitBins[binIndex].bounds.min = min(splitBins[binIndex].bounds.min, shapeInfos[i].bounds.min);
            splitBins[binIndex].bounds.max = max(splitBins[binIndex].bounds.max, shapeInfos[i].bounds.max);
        }

        SplitBin accumBinsLeft[numBins - 1];
        SplitBin accumBinsRight[numBins - 1];
        accumBinsLeft[0] = splitBins[0];
        accumBinsRight[numBins - 2] = splitBins[numBins - 1];
        for (uint32_t i = 1; i < numBins - 1; i++) {
            accumBinsLeft[i].numShapes = accumBinsLeft[i - 1].numShapes + splitBins[i].numShapes;
            accumBinsLeft[i].bounds.min = min(accumBinsLeft[i - 1].bounds.min, splitBins[i].bounds.min);
            accumBinsLeft[i].bounds.max = max(accumBinsLeft[i - 1].bounds.max, splitBins[i].bounds.max);

            uint32_t rightIndex = numBins - 2 - i;
            accumBinsRight[rightIndex].numShapes = accumBinsRight[rightIndex + 1].numShapes + splitBins[rightIndex + 1].numShapes;
            accumBinsRight[rightIndex].bounds.min = min(accumBinsRight[rightIndex + 1].bounds.min, splitBins[rightIndex + 1].bounds.min);
            accumBinsRight[rightIndex].bounds.max = max(accumBinsRight[rightIndex + 1].bounds.max, splitBins[rightIndex + 1].bounds.max);
        }

        float costs[numBins - 1];
        for (uint32_t i = 0; i < numBins - 1; i++) {
            constexpr float costTraverse = 1.0f;
            float n0 = static_cast<float>(accumBinsLeft[i].numShapes);
            float a0 = accumBinsLeft[i].bounds.getSurfaceArea();
            float n1 = static_cast<float>(accumBinsRight[i].numShapes);
            float a1 = accumBinsRight[i].bounds.getSurfaceArea();
            costs[i] = costTraverse + n0 * a0 + n1 * a1;
        }

        float minSplitCost = costs[0];
        uint32_t minCostSplitIndex = 0;
        for (uint32_t i = 1; i < numBins - 1; i++) {
            if (costs[i] < minSplitCost) {
                minSplitCost = costs[i];
                minCostSplitIndex = i;
            }
        }

        float leafCost = static_cast<float>(numShapes) * node.bounds.getSurfaceArea();
        if (numShapes <= maxShapesPerLeaf_ && minSplitCost >= leafCost) {
            // Not worth is splitting any further
            node.firstShapeIndex = static_cast<uint32_t>(orderedShapes_.size());
            node.numShapes = static_cast<uint32_t>(numShapes);
            for (size_t i = left; i < right; i++) {
                orderedShapes_.push_back(shapes[shapeInfos[i].shapeIndex]);
            }
            return nodeIndex;
        }

        auto middleIter = std::partition(shapeInfos.begin() + left, shapeInfos.begin() + right,
            [&](const ShapeInfo& info) {
                size_t binIndex = static_cast<size_t>(k1 * (info.centroid[splitDimension] - k0));
                return binIndex <= minCostSplitIndex;
            });
        middle = static_cast<uint32_t>(middleIter - shapeInfos.begin());
    }

    node.splitAxis = static_cast<uint8_t>(splitDimension);
    node.childIndices[0] = buildInternal(nodes, shapes, shapeInfos, left, middle);
    node.childIndices[1] = buildInternal(nodes, shapes, shapeInfos, middle, right);

    return nodeIndex;
}

uint32_t BVH::flattenTree(uint32_t rootIndex, const std::vector<BuildNode>& buildNodes) {
    const BuildNode& buildNode = buildNodes[rootIndex];
    uint32_t nodeIndex = static_cast<uint32_t>(linearNodes_.size());
    LinearNode& linearNode = linearNodes_.emplace_back();
    linearNode.bounds = buildNode.bounds;

    if (buildNode.isLeaf()) {
        assert(buildNode.numShapes <= std::numeric_limits<uint16_t>::max());
        linearNode.firstShapeIndex = buildNode.firstShapeIndex;
        linearNode.numShapes = static_cast<uint16_t>(buildNode.numShapes);
    }
    else {
        linearNode.numShapes = 0;
        linearNode.splitAxis = buildNode.splitAxis;
        flattenTree(buildNode.childIndices[0], buildNodes);
        linearNode.secondChildOffset = flattenTree(buildNode.childIndices[1], buildNodes);
    }

    return nodeIndex;
}

} // namespace pt
