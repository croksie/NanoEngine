#include "platform/window/window.h"

#include <stdexcept>

#include "utils/log.h"
#include "platform/input/input.h"


namespace midgard::platform {

Window::Window() {
    if (!glfwInit()) {
        ENGINE_LOG_CRITICAL("Error while initializing GLFW");
        throw std::runtime_error("Error while initializing GLFW");
    }
}

void Window::initializeWindow(const int width, const int height, const char* title) {
    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto* instance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        
        if (instance && instance->m_onSizeChanged) {
            instance->m_onSizeChanged(width, height);
        }
    });

    if (!m_window) {
        glfwTerminate();
        ENGINE_LOG_CRITICAL("Error while creating window");
        throw std::runtime_error("Error while creating window");
    }
    
    ENGINE_LOG_DEBUG("Window created");

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input::init(m_window);
}

void Window::setCursorMode(bool disabled) {
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR, disabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

Window::~Window() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
    ENGINE_LOG_DEBUG("Window deleted");
}

} // namespace midgard::platform