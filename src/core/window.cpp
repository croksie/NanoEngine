#include "core/window.h"

#include <spdlog/spdlog.h>


Window::Window(const int width, const int height, const char* title) {

    if (!glfwInit()){
        spdlog::critical("Error while initializing GLFW");
        throw std::runtime_error("Error while initializing GLFW");
    }

    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if(!m_window){
        glfwTerminate();
        spdlog::critical("Error while creating window in window.cpp");
        throw std::runtime_error("Error while creating window");
    }
    glfwMakeContextCurrent(m_window);
}
Window::~Window()
{
    glfwTerminate();
}