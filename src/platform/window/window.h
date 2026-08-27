#pragma once
#include <functional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace midgard::platform {

using SizeCallback = std::function<void(int width, int height)>;

class Window {
public:
    Window();
    ~Window();

    void initializeWindow(const int width, const int height, const char* title);
    void setWindowSizeCallback(SizeCallback cb) { m_onSizeChanged = cb; }

    void* getNativeHandle() const { return static_cast<void*>(m_window); }
    bool windowSouldClose() const { return glfwWindowShouldClose(m_window); }
    void setCursorMode(bool disabled);

private:
    GLFWwindow* m_window = nullptr;
    SizeCallback m_onSizeChanged;
};

} // namespace midgard::platform