#pragma once
#include "core/math/math.h"
#include "core/config.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera() = default;
    Camera(const CameraConfig& config);

    glm::mat4 getProjectionMatrix() { return m_projection; }
    glm::mat4 getViewMatrix() { return m_view; }

    void setPosition(Vec3 newPosition) { m_transform.Position = newPosition; };
private:
    CameraConfig m_config;

    Transform m_transform;
    glm::mat4 m_projection;
    glm::mat4 m_view;


    float m_fov = 90;
};