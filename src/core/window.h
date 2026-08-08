#pragma once
#include <GLFW/glfw3.h>



class Window {
public:
    void* getNativeHandle() const { return static_cast<void*>(m_window); }

    bool windowSouldClose() const { return glfwWindowShouldClose(m_window); }

    Window(const int width, const int height, const char* title);
    ~Window();

private:
    GLFWwindow* m_window;

};