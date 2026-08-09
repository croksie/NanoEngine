#include "core/rhi/opengl/opengl_rhi.h"


void OpenGLRHI::Initialize(Window* window)
{
    m_window = window;
    

     while (!m_window->windowSouldClose())
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(static_cast<GLFWwindow*>(m_window->getNativeHandle()));

        /* Poll for and process events */
        glfwPollEvents();
    }
}