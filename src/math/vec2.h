#pragma once
#include <cmath>

#include <glm/glm.hpp>

namespace midgard::math {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr Vec2() : x(0.0f), y(0.0f) {}
    constexpr Vec2(float scalar) : x(scalar), y(scalar) {}
    constexpr Vec2(float x, float y) : x(x), y(y) {}
    Vec2(const glm::vec2& v) : x(v.x), y(v.y) {}

    // Operators
    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator-() const { return {-x, -y}; }
    Vec2 operator*(float scalar) const { return {x * scalar, y * scalar}; }
    Vec2 operator*(const Vec2& other) const { return {x * other.x, y * other.y}; }
    Vec2 operator/(float scalar) const { return {x / scalar, y / scalar}; }
    Vec2 operator/(const Vec2& other) const { return {x / other.x, y / other.y}; }

    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }

    Vec2 normalized() const {
        float len = length();
        return len > 0.0f ? *this / len : Vec2();
    }

    // GLM cast
    operator glm::vec2() const { return glm::vec2(x, y); }
    glm::vec2 toGlm() const { return glm::vec2(x, y); }
};

inline Vec2 operator*(float scalar, const Vec2& v) {
    return v * scalar;
}

} // namespace midgard::math