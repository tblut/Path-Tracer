#include "VecMath.h"
using namespace pt;

#include <iostream>
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
    std::cout << "a*3  " << isApproxEqual(a * 3, Vec3(3, 6, 9)) << '\n';
    std::cout << "3*a  " << isApproxEqual(3 * a, Vec3(3, 6, 9)) << '\n';
    std::cout << "a/b  " << isApproxEqual(a / b, Vec3(0.5f, 1, 1.5f)) << '\n';
    std::cout << "a/2  " << isApproxEqual(a / 2, Vec3(0.5f, 1, 1.5f)) << '\n';

    std::cout << "a+=b  " << isApproxEqual(a += b, Vec3(3, 4, 5)) << '\n';
    std::cout << "a-=b  " << isApproxEqual(a -= b, Vec3(1, 2, 3)) << '\n';
    std::cout << "a*=b  " << isApproxEqual(a *= b, Vec3(2, 4, 6)) << '\n';
    std::cout << "a/=b  " << isApproxEqual(a /= b, Vec3(1, 2, 3)) << '\n';
    std::cout << "a*=2  " << isApproxEqual(a *= 2, Vec3(2, 4, 6)) << '\n';
    std::cout << "a/=2  " << isApproxEqual(a /= 2, Vec3(1, 2, 3)) << '\n';

    std::cout << "min(2,5)  " << isApproxEqual(min(2.f, 5.f), 2) << '\n';
    std::cout << "max(2,5)  " << isApproxEqual(max(2.f, 5.f), 5) << '\n';
    std::cout << "min(a,b)  " << isApproxEqual(min(a, b), Vec3(1, 2, 2)) << '\n';
    std::cout << "max(a,b)  " << isApproxEqual(max(a, b), Vec3(2, 2, 3)) << '\n';
    std::cout << "abs((-1, 2, -3))  " << isApproxEqual(abs(Vec3(-1, 2, -3)), Vec3(1, 2, 3)) << '\n';
    std::cout << "floor((1.2, 2.6, 3.7))  " << isApproxEqual(floor(Vec3(1.2f, 2.6f, 3.7f)), Vec3(1, 2, 3)) << '\n';
    std::cout << "ceil((1.2, 2.6, 3.7))  " << isApproxEqual(ceil(Vec3(1.2f, 2.6f, 3.7f)), Vec3(2, 3, 4)) << '\n';


    std::cout << "dot(a,b)  " << isApproxEqual(dot(a, b), 12) << '\n';
    std::cout << "lengthSq(a)  " << isApproxEqual(lengthSq(a), 14) << '\n';
    std::cout << "length(a)  " << isApproxEqual(length(a), 3.741657386773f) << '\n';
    std::cout << "normalize(a)  " << isApproxEqual(normalize(a),
        Vec3(1/ 3.741657386773f, 2 / 3.741657386773f, 3 / 3.741657386773f)) << '\n';
    std::cout << "cross((1,0,0),(0,0,-1))  " << isApproxEqual(cross(Vec3{ 1,0,0 }, Vec3{0,0,-1}), Vec3(0, 1, 0)) << '\n';
    
    return 0;
}
