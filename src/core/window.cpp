#include "core/Window.h"

#include <utils/Log.h>


Window::Window(const int width, const int height, const char* title) {

    if (!glfwInit()){
       ENGINE_LOG_CRITICAL("Error while initializing GLFW");
        throw std::runtime_error("Error while initializing GLFW");
    }

    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!m_window){
        glfwTerminate();
        ENGINE_LOG_CRITICAL("Error while creating window");
        throw std::runtime_error("Error while creating window");
    }
    glfwMakeContextCurrent(m_window);
    ENGINE_LOG_DEBUG("Window created");
}
Window::~Window()
{
    glfwTerminate();
    ENGINE_LOG_DEBUG("Window deleted");
}