#pragma once
#include "platform/input/key_codes.h"

#include <GLFW/glfw3.h>

#include <functional>
#include <vector>

class Input {
public:
    static void init(GLFWwindow* window);

    static bool isKeyPressed(KeyCode key);
    static bool isMouseButtonPressed(int button);
    static void getMousePosition(double& x, double& y);

    using KeyCallback = std::function<void(KeyCode key, Action action)>;
    using ScrollCallback = std::function<void(double xOffset, double yOffset)>;

    static void addKeyCallback(KeyCallback callback);

private:
    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    inline static GLFWwindow* m_window = nullptr;
};
