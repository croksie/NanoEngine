#pragma once
#include "core/config.h"
#include "core/math/math.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    Camera() = default;
    Camera(const CameraConfig& config, Vec3 position = Vec3(0.0f, 2.0f, -10.0f));

    glm::mat4 getProjectionMatrix() const { return m_projection; }
    glm::mat4 getViewMatrix() const;

    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void processKeyboard(const Vec3& direction, float deltaTime);

    Vec3 getPosition() const { return m_position; }
    void setPosition(const Vec3& newPosition);

    Vec3 getFront() const { return m_front; }
    Vec3 getRight() const { return m_right; }
    Vec3 getUp() const { return m_up; }

    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

    float getSensitivity() const { return m_sensitivity; }
    void setSensitivity(float sensitivity) { m_sensitivity = sensitivity; }

private:
    void updateCameraVectors();

    CameraConfig m_config;

    Vec3 m_position = Vec3(0.0f, 2.0f, -10.0f);
    Vec3 m_front = Vec3(0.0f, 0.0f, 1.0f);
    Vec3 m_up = Vec3(0.0f, 1.0f, 0.0f);
    Vec3 m_right = Vec3(1.0f, 0.0f, 0.0f);
    Vec3 m_worldUp = Vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 m_projection = glm::mat4(1.0f);

    float m_yaw = 90.0f;
    float m_pitch = 0.0f;
    float m_speed = 6.0f;
    float m_sensitivity = 0.1f;
};