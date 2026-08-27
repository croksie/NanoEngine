#pragma once
#include <cmath>

#include <glm/glm.hpp>

#include "math/vec2.h"

namespace midgard::math {

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}
    constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    constexpr Vec3(const Vec2& v, float z) : x(v.x), y(v.y), z(z) {}
    Vec3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}

    // Operators
    Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vec3 operator*(const Vec3& other) const { return {x * other.x, y * other.y, z * other.z}; }
    Vec3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    Vec3 operator/(const Vec3& other) const { return {x / other.x, y / other.y, z / other.z}; }

    Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vec3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    bool operator==(const Vec3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(const Vec3& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float lengthSquared() const { return x * x + y * y + z * z; }

    Vec3 normalized() const {
        float len = length();
        return len > 0.0f ? *this / len : Vec3();
    }

    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    // GLM cast
    operator glm::vec3() const { return glm::vec3(x, y, z); }
    glm::vec3 toGlm() const { return glm::vec3(x, y, z); }
};

inline Vec3 operator*(float scalar, const Vec3& v) {
    return v * scalar;
}

} // namespace midgard::math