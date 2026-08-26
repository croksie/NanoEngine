#pragma once
#include <GLFW/glfw3.h>

enum class KeyCode {
    Z = GLFW_KEY_W, 
    Q = GLFW_KEY_A, 
    S = GLFW_KEY_S, 
    D = GLFW_KEY_D, 
    Escape = GLFW_KEY_ESCAPE, 
    Space = GLFW_KEY_SPACE
};

enum class Action { 
    Press = GLFW_PRESS, 
    Release = GLFW_RELEASE, 
    Repeat = GLFW_REPEAT 
};