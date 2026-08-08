#include <core/Window.h>
#include <rendering/Renderer.h>
#include <utils/Log.h>

int main() 
{
    ENGINE_LOG_INFO("NanoEngine starting ...");

    Window window(1280, 720, "NanoEngine");

    Renderer renderer;
    renderer.Initialize(&window);

    return 0;
    
}
