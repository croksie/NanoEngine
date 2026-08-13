#include <core/window.h>
#include <rendering/renderer.h>
#include <utils/log.h>

int main() 
{

    spdlog::set_level(spdlog::level::info);

    ENGINE_LOG_INFO("NanoEngine starting ...");

    Window window;

    Renderer renderer;
    renderer.initialize(&window);

    while(!window.windowSouldClose()) {
        renderer.render();
    }


    return 0;
    
}
