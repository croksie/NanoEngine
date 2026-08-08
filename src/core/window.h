#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>




class Window {
public:
    void initializeWindow(const int width, const int height, const char* title);

    void* getNativeHandle() const { return static_cast<void*>(m_window); }
    bool windowSouldClose() const { return glfwWindowShouldClose(m_window); }

    Window();
    ~Window();

private:
    GLFWwindow* m_window;

};