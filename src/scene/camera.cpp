#include "scene/camera.h"
#include <algorithm>

Camera::Camera(const CameraConfig& config, Vec3 position) : m_config(config), m_position(position) {
    m_projection = glm::perspective(glm::radians(config.fov), config.widthResolution / config.heightResolution, 0.1f, 100.0f);
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt<float, glm::defaultp>(m_position, m_position + m_front, m_up);
}

void Camera::setPosition(const Vec3& newPosition) {
    m_position = newPosition;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= m_sensitivity;
    yOffset *= m_sensitivity;

    m_yaw   += xOffset;
    m_pitch += yOffset;

    if (constrainPitch) {
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
}

void Camera::processKeyboard(const Vec3& direction, float deltaTime) {
    float velocity = m_speed * deltaTime;
    m_position += direction * velocity;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    glm::vec3 normFront = glm::normalize(front);
    m_front = Vec3(normFront.x, normFront.y, normFront.z);

    glm::vec3 worldUp = m_worldUp;
    glm::vec3 right = glm::normalize(glm::cross(normFront, worldUp));
    m_right = Vec3(right.x, right.y, right.z);

    glm::vec3 up = glm::normalize(glm::cross(right, normFront));
    m_up = Vec3(up.x, up.y, up.z);
}
