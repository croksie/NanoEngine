#pragma once
#include "GLFW/glfw3.h"



class Window {
public:
    void* getNativeHandle() const { return static_cast<void*>(m_window); }

    Window(const int width, const int height, const char* title);


private:
    GLFWwindow* m_window;

};