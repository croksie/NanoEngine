#pragma once

#include <string>
#include <cstdint>

enum class GraphicsAPI {
    OpenGL,
    Vulkan
};

struct EngineConfig {
    // Window
    uint32_t windowWidth = 1280;
    uint32_t windowHeight = 720;
    std::string windowTitle = "NanoEngine";
    bool fullscreen = false;
    bool resizable = true;

    // Render
    GraphicsAPI api = GraphicsAPI::Vulkan;
    bool vsync = true;
    uint32_t maxFramesInFlight = 3;
};
