#include <core/window.h>
#include <rendering/renderer.h>
#include <utils/log.h>

int main() 
{

    spdlog::set_level(spdlog::level::debug);

    ENGINE_LOG_INFO("NanoEngine starting ...");

    Window window;

    Renderer renderer;
    renderer.Initialize(&window);

    return 0;
    
}
