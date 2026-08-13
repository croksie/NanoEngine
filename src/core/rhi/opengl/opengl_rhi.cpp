#include "core/rhi/opengl/opengl_rhi.h"

#include "utils/log.h"
#include "opengl_rhi.h"


void OpenGLRHI::initialize(Window* window)
{
    m_window = window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    m_window->initializeWindow(1280, 720, "NanoEngine");
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window->getNativeHandle()));

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        ENGINE_LOG_CRITICAL("Failed to initialize OpenGL context");
        throw std::runtime_error("Failed to initialize OpenGL context");
    }
    
    ENGINE_LOG_INFO("OpenGL Context Initialized: {}", (const char*)glGetString(GL_VERSION));

    glViewport(0, 0, 1280, 720);
   
}


/// @brief Do nothing in OpenGl
void OpenGLRHI::beginFrame() {
    ENGINE_LOG_TRACE("OpenGLRHI::BeginFrame");
}

/// @brief Swap buffers
void OpenGLRHI::endFrame() {
     glfwSwapBuffers(static_cast<GLFWwindow*>(m_window->getNativeHandle()));
     glfwPollEvents();
     ENGINE_LOG_TRACE("OpenGLRHI::Buffer swaped");
}

/// @brief Fill the buffer with 0.0f 0.0f 0.0f
void OpenGLRHI::clear() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ENGINE_LOG_TRACE("OpenGLRHI::Buffer cleared");
}


/******** BUFFER ********/

/// @brief Create an OpenGl VBO
/// @param vertices An array of flaot with all the verticles
/// @param size The size of the array
/// @return An abstarct class buffer who contains an OpenGl Buffer
std::shared_ptr<Buffer> OpenGLRHI::createBuffer(float vertices[], size_t size) {
    ENGINE_LOG_TRACE("OpenGLRHI::Creating buffer...");
    return std::make_shared<OpenGLBuffer>(size, vertices);

}

/// @brief Bind a VBO to a VAO
/// @param pipeline The abstarction that contains the VAO
/// @param buffer The abstarction that contains the VBO
void OpenGLRHI::bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) {
    if(pipeline == nullptr) {
        ENGINE_LOG_CRITICAL("Pipeline is null");
        throw std::runtime_error("Pipeline is null");
    }
    if(buffer == nullptr) {
        ENGINE_LOG_CRITICAL("Buffer is null");
        throw std::runtime_error("Buffer is null");
    }
    pipeline->bindVertexBuffer(buffer);
    ENGINE_LOG_TRACE("OpenGLRHI::Vertex buffer binded");
}

/******** PIPELINE/SHADER ********/

/// @brief Create a VAO
/// @return An abstarct class Pipeline who contains an OpenGl Array object
std::shared_ptr<Pipeline> OpenGLRHI::createPipeline(PipelineInfo& info) {
    ENGINE_LOG_TRACE("OpenGLRHI::Creating pipeline...");
    return std::make_shared<OpenGLPipeline>(info);
}


void OpenGLRHI::bindPipeline(Pipeline* pipeline) {
    if(pipeline == nullptr) {
        ENGINE_LOG_CRITICAL("Not a valid pipeline");
        throw std::runtime_error("Not a valid pipeline");
    }
    ENGINE_LOG_TRACE("OpenGLRHI::Binding pipeline...");
    GLuint shaderProgram = static_cast<OpenGLPipeline*>(pipeline)->getShaderProgramID();
    glUseProgram(shaderProgram); 
    ENGINE_LOG_TRACE("OpenGLRHI::Pipeline binded");
}


void OpenGLRHI::draw(std::shared_ptr<Pipeline> pipeline) {
    ENGINE_LOG_TRACE("OpenGLRHI::Drawing...");
    OpenGLPipeline* openGlPipeline = static_cast<OpenGLPipeline*>(pipeline.get());
    
    glBindVertexArray(openGlPipeline->getVertexArrayID());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_LOG_TRACE("OpenGLRHI::Drawed");
}
