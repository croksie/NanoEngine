#include "scene/camera.h"

Camera::Camera(const CameraConfig& config)  : m_config(config) {

    m_projection = glm::perspective(glm::radians(config.fov), config.widthResolution / config.heightResolution, 0.1f, 100.0f);
    m_view = glm::translate(glm::mat4(1.0f) , static_cast<glm::vec3>(m_transform.Position)); 
}

