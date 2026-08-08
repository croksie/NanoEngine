#include <spdlog/spdlog.h>
#include <core/window.h>
#include <rendering/Renderer.h>

int main() 
{
    spdlog::info("NanoEngine starting ...");
    Window window(1280, 720, "NanoEngine");

    Renderer renderer;
    renderer.Initialize(&window);

    return 0;
    
}
