#pragma once
#include <glm/glm.hpp>

struct Vec2
{
    float x;
    float y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2(const glm::vec2& v) : x(v.x), y(v.y) {}

    Vec2 operator+(const Vec2& other) const {
        return Vec2{x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2{x - other.x, y - other.y};
    }

    Vec2 operator-() const {
        return Vec2{-x, -y};
    }

    Vec2 operator*(float scalar) const {
        return Vec2{x * scalar, y * scalar};
    }

    Vec2 operator/(float scalar) const {
        return Vec2{x / scalar, y / scalar};
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    operator glm::vec2 () const {
        return glm::vec2(x, y);
    }
};

struct Vec3
{
    float x;
    float y;
    float z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}

    Vec3 operator+(const Vec3& other) const {
        return Vec3{x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3{x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator-() const {
        return Vec3{-x, -y, -z};
    }

    Vec3 operator*(float scalar) const {
        return Vec3{x * scalar, y * scalar, z * scalar};
    }

    Vec3 operator/(float scalar) const {
        return Vec3{x / scalar, y / scalar, z / scalar};
    }

    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    operator glm::vec3 () const {
        return glm::vec3(x, y, z);
    }
};

struct Vec4
{
    float x;
    float y;
    float z;
    float w;

    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vec4(const glm::vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

    Vec4 operator+(const Vec4& other) const {
        return Vec4{x + other.x, y + other.y, z + other.z, w + other.w};
    }

    Vec4 operator-(const Vec4& other) const {
        return Vec4{x - other.x, y - other.y, z - other.z, w - other.w};
    }

    Vec4 operator-() const {
        return Vec4{-x, -y, -z, -w};
    }

    Vec4 operator*(float scalar) const {
        return Vec4{x * scalar, y * scalar, z * scalar, w * scalar};
    }

    Vec4 operator/(float scalar) const {
        return Vec4{x / scalar, y / scalar, z / scalar, w / scalar};
    }

    Vec4& operator+=(const Vec4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }

    Vec4& operator-=(const Vec4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }

    operator glm::vec4 () const {
        return glm::vec4(x, y, z, w);
    }
};
