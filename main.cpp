#include "core/window.h"
#include "rendering/renderer.h"
#include "utils/log.h"

int main() 
{
    #ifdef DEBUG
        spdlog::set_level(spdlog::level::debug);
        ENGINE_LOG_DEBUG("Debug mode enabled");
    #else
        spdlog::set_level(spdlog::level::info);
    #endif

    ENGINE_LOG_INFO("NanoEngine starting ...");

    Window window;

    Renderer renderer;
    renderer.initialize(&window);

    static auto lastTime = std::chrono::high_resolution_clock::now();
    static int frameCount = 0;

    while(!window.windowSouldClose()) {
        renderer.render();

        // FPS Counter
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float elapsed = std::chrono::duration<float>(currentTime - lastTime).count();

        if (elapsed >= 1.0f) {
            ENGINE_LOG_INFO("FPS: {} ({} ms/frame)", frameCount ,1000.0f / frameCount);
            frameCount = 0;
            lastTime = currentTime;
        }
    }
    ENGINE_LOG_INFO("NanoEngine shutting down ...");
    renderer.shutdown();

    return 0;
    
}
