#include <spdlog/spdlog.h>
#include <RHI/window.h>

int main() 
{
    spdlog::info("NanoEngine starting ...");
    const Window window(1280, 720, "NanoEngine");
    return 0;
    
}
