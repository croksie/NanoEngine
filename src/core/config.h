#pragma once

#include <string>
#include <cstdint>

namespace midgard::core {

enum class GraphicsAPI {
    OpenGL,
    Vulkan
};

struct EngineConfig {
    // Window
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    std::string windowTitle = "Midgard Engine";
    bool fullscreen = false;
    bool resizable = true;

    // Render
    GraphicsAPI api = GraphicsAPI::Vulkan;
    bool vsync = true;
    uint32_t maxFramesInFlight = 3;
};

struct CameraConfig {
    float widthResolution = 1280.0f;
    float heightResolution = 720.0f;
    float fov = 90.0f;
};

} // namespace midgard::core


