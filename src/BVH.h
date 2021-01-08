#pragma once

#include "BoundingBox.h"

#include <vector>
#include <functional>

namespace pt {

class Shape;
struct Ray;

// See: On fast Construction of SAH-based Bounding Volume Hierarchies (2007), Wald
class BVH {
public:
    struct LinearNode {
        constexpr bool isLeaf() const {
            return numShapes > 0;
        }

        BoundingBox bounds;
        union {
            uint32_t firstShapeIndex;
            uint32_t secondChildOffset;
        };
        uint16_t numShapes;
        uint8_t splitAxis;
    };
    using TraversalCallback = std::function<bool(const LinearNode&)>;

    BVH(const std::vector<const Shape*>& shapes, uint32_t maxShapesPerLeaf);
    RayHit intersect(Ray ray) const;

    // Depth-first traversal (for testing purposes)
    void traverse(const TraversalCallback& callback) const;

private:
    struct BuildNode {
        constexpr bool isLeaf() const {
            return splitAxis == std::numeric_limits<uint8_t>::max();
        }

        union {
            uint32_t childIndices[2];
            struct {
                uint32_t firstShapeIndex;
                uint32_t numShapes;
            };
        };
        BoundingBox bounds;
        uint8_t splitAxis;
    };

    struct ShapeInfo {
        uint32_t shapeIndex;
        BoundingBox bounds;
        Vec3 centroid;
    };

    uint32_t buildInternal(std::vector<BuildNode>& nodes, const std::vector<const Shape*>& shapes,
        std::vector<ShapeInfo>& shapeInfos, uint32_t left, uint32_t right);
    uint32_t flattenTree(uint32_t rootIndex, const std::vector<BuildNode>& buildNodes);

    std::vector<const Shape*> orderedShapes_;
    std::vector<LinearNode> linearNodes_;
    uint32_t rootNodeIndex_;
    uint32_t maxShapesPerLeaf_;
};

} // namespace pt
