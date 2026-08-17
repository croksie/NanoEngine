#include "core/window.h"

#include "utils/log.h"


Window::Window() {

    if (!glfwInit()){
       ENGINE_LOG_CRITICAL("Error while initializing GLFW");
        throw std::runtime_error("Error while initializing GLFW");
    }
}


void Window::initializeWindow(const int width, const int height, const char * title)
{
    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!m_window){
        glfwTerminate();
        ENGINE_LOG_CRITICAL("Error while creating window");
        throw std::runtime_error("Error while creating window");
    }
    ENGINE_LOG_DEBUG("Window created");
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
    ENGINE_LOG_DEBUG("Window deleted");
}