#include <memory>
#include <chrono>

#include "platform/window/window.h"
#include "core/config.h"
#include "rendering/renderer.h"
#include "utils/log.h"

int main() 
{
    auto config = std::make_shared<midgard::core::EngineConfig>();
    config->api = midgard::core::GraphicsAPI::Vulkan;

    #ifdef DEBUG
        spdlog::set_level(spdlog::level::debug);
        ENGINE_LOG_DEBUG("Debug mode enabled");
    #else
        spdlog::set_level(spdlog::level::info);
    #endif

    ENGINE_LOG_INFO("Midgard Engine starting ...");

    midgard::platform::Window window;

    midgard::render::Renderer renderer;
    renderer.initialize(&window, config);

    static auto lastTime = std::chrono::high_resolution_clock::now();
    static int frameCount = 0;

    while(!window.windowSouldClose()) {
        renderer.render();

        // FPS Counter
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsed = std::chrono::duration<float>(currentTime - lastTime).count();

        if (elapsed >= 1.0f) {
            ENGINE_LOG_INFO("FPS: {} ({} ms/frame)", frameCount, 1000.0f / frameCount);
            frameCount = 0;
            lastTime = currentTime;
        }
    }
    ENGINE_LOG_INFO("Midgard Engine shutting down ...");
    renderer.shutdown();

    return 0;
}
