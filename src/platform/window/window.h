#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "platform/input/input.h"

#include <functional>


using SizeCallback = std::function<void(int width, int height)>;

class Window {
public:
    void initializeWindow(const int width, const int height, const char* title);
    void setWindowSizeCallback(SizeCallback cb) { m_onSizeChanged = cb; }

    void* getNativeHandle() const { return static_cast<void*>(m_window); }
    bool windowSouldClose() const { return glfwWindowShouldClose(m_window); }
    void setCursorMode(bool disabled);

    Window();
    ~Window();

private:
    GLFWwindow* m_window;
    SizeCallback m_onSizeChanged;
};