#include "platform/input/input.h"

#include "utils/log.h"

namespace midgard::platform {

void Input::init(GLFWwindow *window) {
    m_window = window;

    glfwSetKeyCallback(window, glfwKeyCallback);
    //glfwSetScrollCallback(window, glfwScrollCallback);
    
    ENGINE_LOG_INFO("Input system initialized.");
}

bool Input::isKeyPressed(KeyCode key) {
    return glfwGetKey(m_window, static_cast<int>(key)) == static_cast<int>(Action::Press);
}

bool Input::isMouseButtonPressed(int button) {
    return glfwGetMouseButton(m_window, button) == static_cast<int>(Action::Press);
}

void Input::getMousePosition(double &x, double &y) {
    glfwGetCursorPos(m_window, &x, &y);
}

void Input::addKeyCallback(const KeyCallback& callback) {
    m_keyCallbacks.push_back(callback);
}

void Input::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void)window;
    (void)scancode;
    (void)mods;
    KeyCode keyCode = static_cast<KeyCode>(key);
    Action actionType = static_cast<Action>(action);

    for (const auto& callback : m_keyCallbacks) {
        callback(keyCode, actionType);
    }
}

} // namespace midgard::platform
