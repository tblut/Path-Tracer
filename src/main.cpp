#include "VecMath.h"
#include "stb_image_write.h"
using namespace pt;

#include <iostream>
#include <vector>
#include <cassert>

bool isApproxEqual(float a, float b, float eps = 1.0e-8f) {
    return std::abs(a - b) < eps;
}

bool isApproxEqual(const Vec3& a, const Vec3& b, float eps = 1.0e-8f) {
    return isApproxEqual(a.x, b.x, eps)
        && isApproxEqual(a.y, b.y, eps)
        && isApproxEqual(a.z, b.z, eps);
}

int main(int argc, char** argv) {
    Vec3 a(1.0f, 2.0f, 3.0f);
    Vec3 b(2.0f, 2.0f, 2.0f);

    std::cout << "a+b  " << isApproxEqual(a + b, Vec3(3, 4, 5)) << '\n';
    std::cout << "-a   " << isApproxEqual(-a, Vec3(-1, -2, -3)) << '\n';
    std::cout << "a-b  " << isApproxEqual(a - b, Vec3(-1, 0, 1)) << '\n';
    std::cout << "a*b  " << isApproxEqual(a * b, Vec3(2, 4, 6)) << '\n';
    std::cout << "a*3  " << isApproxEqual(a * 3.f, Vec3(3, 6, 9)) << '\n';
    std::cout << "3*a  " << isApproxEqual(3.f * a, Vec3(3, 6, 9)) << '\n';
    std::cout << "a/b  " << isApproxEqual(a / b, Vec3(0.5f, 1, 1.5f)) << '\n';
    std::cout << "a/2  " << isApproxEqual(a / 2.f, Vec3(0.5f, 1, 1.5f)) << '\n';

    std::cout << "a+=b  " << isApproxEqual(a += b, Vec3(3, 4, 5)) << '\n';
    std::cout << "a-=b  " << isApproxEqual(a -= b, Vec3(1, 2, 3)) << '\n';
    std::cout << "a*=b  " << isApproxEqual(a *= b, Vec3(2, 4, 6)) << '\n';
    std::cout << "a/=b  " << isApproxEqual(a /= b, Vec3(1, 2, 3)) << '\n';
    std::cout << "a*=2  " << isApproxEqual(a *= 2.f, Vec3(2, 4, 6)) << '\n';
    std::cout << "a/=2  " << isApproxEqual(a /= 2.f, Vec3(1, 2, 3)) << '\n';

    std::cout << "min(2,5)  " << isApproxEqual(min(2.f, 5.f), 2) << '\n';
    std::cout << "max(2,5)  " << isApproxEqual(max(2.f, 5.f), 5) << '\n';
    std::cout << "min(a,b)  " << isApproxEqual(min(a, b), Vec3(1, 2, 2)) << '\n';
    std::cout << "max(a,b)  " << isApproxEqual(max(a, b), Vec3(2, 2, 3)) << '\n';
    std::cout << "clamp(2,3,5)  " << isApproxEqual(clamp(2.f, 3.f, 5.f), 3) << '\n';
    std::cout << "clamp(6,3,5)  " << isApproxEqual(clamp(6.f, 3.f, 5.f), 5) << '\n';
    std::cout << "clamp(a,1.5,2.5)  " << isApproxEqual(clamp(a, 1.5f, 2.5f), Vec3(1.5f, 2.0f, 2.5f)) << '\n';
    std::cout << "saturate(3)  " << isApproxEqual(saturate(3.f), 1) << '\n';
    std::cout << "saturate(-4)  " << isApproxEqual(saturate(-4.0f), 0) << '\n';
    std::cout << "saturate(0.5)  " << isApproxEqual(saturate(0.5f), 0.5f) << '\n';
    std::cout << "saturate(a)  " << isApproxEqual(saturate(a), Vec3(1.f, 1.f, 1.f)) << '\n';
    std::cout << "abs((-1, 2, -3))  " << isApproxEqual(abs(Vec3(-1, 2, -3)), Vec3(1, 2, 3)) << '\n';
    std::cout << "floor((1.2, 2.6, 3.7))  " << isApproxEqual(floor(Vec3(1.2f, 2.6f, 3.7f)), Vec3(1, 2, 3)) << '\n';
    std::cout << "ceil((1.2, 2.6, 3.7))  " << isApproxEqual(ceil(Vec3(1.2f, 2.6f, 3.7f)), Vec3(2, 3, 4)) << '\n';
    std::cout << "degrees(pi)  " << isApproxEqual(degrees(pi<float>), 180) << '\n';
    std::cout << "radians(180)  " << isApproxEqual(radians(180.0f), pi<float>) << '\n';
    std::cout << "lerp(2, 6, 0.25)  " << isApproxEqual(lerp(2.f, 6.f, 0.25f), 3.0f) << '\n';
    std::cout << "lerp((2, 2, 2), (6, 6, 6), 0.25)  " << isApproxEqual(lerp(Vec3(2.f), Vec3(6.f), 0.25f), Vec3(3.0f)) << '\n';

    std::cout << "dot(a,b)  " << isApproxEqual(dot(a, b), 12) << '\n';
    std::cout << "lengthSq(a)  " << isApproxEqual(lengthSq(a), 14) << '\n';
    std::cout << "length(a)  " << isApproxEqual(length(a), 3.741657386773f) << '\n';
    std::cout << "normalize(a)  " << isApproxEqual(normalize(a),
        Vec3(1/ 3.741657386773f, 2 / 3.741657386773f, 3 / 3.741657386773f)) << '\n';
    std::cout << "cross((1,0,0),(0,0,-1))  " << isApproxEqual(cross(Vec3{ 1,0,0 }, Vec3{0,0,-1}), Vec3(0, 1, 0)) << '\n';
    std::cout << "reflect((1,-1,0),(0,1,0))  " << isApproxEqual(reflect(Vec3(1, -1, 0), Vec3(0, 1, 0)), Vec3(1, 1, 0)) << '\n';
    std::cout << "refract((1,-1,0),(0,1,0),1)  " << isApproxEqual(refract(Vec3(1, -1, 0), Vec3(0, 1, 0), 1.0f), Vec3(1, -1, 0)) << '\n';

    std::vector<uint32_t> pixels(256 * 128, 0xffff00ff);
    stbi_write_png("test.png", 256, 128, 4, pixels.data(), 4);

    return 0;
}
