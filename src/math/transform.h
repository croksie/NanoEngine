#pragma once
#include <numbers>

#include "math/vec3.h"
#include "math/mat4.h"

namespace midgard::math {

struct Transform {
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 rotation = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 scale    = Vec3(1.0f, 1.0f, 1.0f);

    Mat4 toMatrix() const {
        Mat4 m = Mat4::identity();
        m = Mat4::translate(m, position);
        m = Mat4::rotate(m, rotation.x * (std::numbers::pi_v<float> / 180.0f), Vec3(1.0f, 0.0f, 0.0f));
        m = Mat4::rotate(m, rotation.y * (std::numbers::pi_v<float> / 180.0f), Vec3(0.0f, 1.0f, 0.0f));
        m = Mat4::rotate(m, rotation.z * (std::numbers::pi_v<float> / 180.0f), Vec3(0.0f, 0.0f, 1.0f));
        m = Mat4::scale(m, scale);
        return m;
    }
};

} // namespace midgard::math