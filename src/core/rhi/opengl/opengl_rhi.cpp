#include "core/rhi/opengl/opengl_rhi.h"

#include "utils/log.h"
#include "opengl_rhi.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void OpenGLRHI::initialize(Window* window)
{
    m_window = window;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    m_window->initializeWindow(1280, 720, "NanoEngine");
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_window->getNativeHandle()));
    
    m_window->setWindowSizeCallback([this](int width, int height) {
        this->onWindowResize(width, height);
    });

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        ENGINE_LOG_CRITICAL("Failed to initialize OpenGL context");
        throw std::runtime_error("Failed to initialize OpenGL context");
    }
    
    ENGINE_LOG_INFO("OpenGLRHI::OpengGL Version {}", (const char*)glGetString(GL_VERSION));
    ENGINE_LOG_INFO("OpenGLRHI::GPU: {}", (const char*)glGetString(GL_RENDERER));

    #ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        (void)source;
        (void)type;
        (void)id;
        (void)length;
        (void)userParam;
        if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
        ENGINE_LOG_WARN("OpenGLRHI::Debug: {}", message);
    }, nullptr);
    #endif

    glfwSwapInterval(1); // V-sync 0-off, 1-on

    glViewport(0, 0, 1280, 720);
    glEnable(GL_SCISSOR_TEST);
    // DepthBuffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Backface-Culling
    glEnable(GL_CULL_FACE);  
    glCullFace(GL_FRONT);  
    glFrontFace(GL_CW);

    m_uniformBuffer = std::make_unique<OpenGLBuffer>(2000, nullptr); // FIXME Change the buffer size

    ENGINE_LOG_INFO("OpenGLRHI::Initialization success");


}

void OpenGLRHI::onWindowResize(int width, int height) {
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

std::shared_ptr<Shader> OpenGLRHI::createShader(ShaderType type, std::string source) {
    return std::make_shared<OpenGLShader>(type, source);
}

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

/******** Uniforms ********/

void OpenGLRHI::setGlobalUniform(const void *data, size_t size) {
    m_uniformBuffer->setData(size, data, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer->getBufferId());
}

void OpenGLRHI::setLocalUniform(const void *data, size_t size) {
    glUniformMatrix4fv(0 , 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data));
}

/******** Draw ********/

void OpenGLRHI::draw(std::shared_ptr<Pipeline> pipeline) {
    ENGINE_LOG_TRACE("OpenGLRHI::Drawing...");
    OpenGLPipeline* openGlPipeline = static_cast<OpenGLPipeline*>(pipeline.get());
    
    glBindVertexArray(openGlPipeline->getVertexArrayID());
    glDrawArrays(GL_TRIANGLES, 0, openGlPipeline->getBindedNumberOfVerticles());
    ENGINE_LOG_TRACE("OpenGLRHI::Drawed");
}
