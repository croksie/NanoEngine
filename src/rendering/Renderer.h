#pragma once
#include <memory>
#include <vector>

#include "bifrost/bifrost.h"
#include "scene/model.h"
#include "scene/camera.h"

namespace midgard::core { 
    struct EngineConfig; 
}

namespace midgard::platform { 
    class Window; 
}

namespace midgard::bifrost { 
    class Buffer; 
    class Texture;
}

namespace midgard::render {

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void initialize(platform::Window* window, std::shared_ptr<core::EngineConfig> config);
    void render();
    void shutdown();

private:
    std::shared_ptr<core::EngineConfig> m_config;
    std::unique_ptr<bifrost::Bifrost> m_rhi;

    scene::Camera m_camera;

    std::vector<scene::Model> models;

    std::vector<bifrost::InstanceData> m_instances = std::vector<bifrost::InstanceData>(900);
    std::shared_ptr<bifrost::Buffer> m_instanceBuffer;
    std::shared_ptr<bifrost::Texture> m_texture;
    void createTestModel();

    float m_lastFrameTime = 0.0f;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_firstMouse = true;
};

} // namespace midgard::render