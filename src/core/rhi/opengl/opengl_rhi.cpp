#include "core/rhi/opengl/opengl_rhi.h"

#include "utils/log.h"
#include "opengl_rhi.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void OpenGLRHI::initialize(Window* window, std::shared_ptr<EngineConfig> config) {
    m_window = window;
    m_config = config;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_RESIZABLE, m_config->resizable);
    // TODO : implement fullscreen mode

    m_window->initializeWindow(m_config->windowWidth, m_config->windowHeight, m_config->windowTitle.c_str());
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

    glfwSwapInterval(m_config->vsync ? 1 : 0);

    glViewport(0, 0, m_config->windowWidth, m_config->windowHeight);
    glEnable(GL_SCISSOR_TEST);
    // DepthBuffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Backface-Culling
    glEnable(GL_CULL_FACE);  
    glCullFace(GL_BACK);  
    glFrontFace(GL_CCW);

    BufferDesc desc{};
    desc.size = 2048;
    desc.type = BufferType::UNIFORM;
    m_uniformBuffer = std::make_unique<OpenGLBuffer>(desc); // FIXME Change the buffer size

    ENGINE_LOG_INFO("OpenGLRHI::Initialization success");


}

void OpenGLRHI::onWindowResize(int width, int height) {
    m_config->windowWidth = width;
    m_config->windowHeight = height;

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
std::shared_ptr<Buffer> OpenGLRHI::createBuffer(BufferDesc& desc) {
    ENGINE_LOG_TRACE("OpenGLRHI::Creating buffer...");
    return std::make_shared<OpenGLBuffer>(desc);
}

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

void OpenGLRHI::bindInstanceBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) {
    if(pipeline == nullptr) {
        ENGINE_LOG_CRITICAL("Pipeline is null");
        throw std::runtime_error("Pipeline is null");
    }
    if(buffer == nullptr) {
        ENGINE_LOG_CRITICAL("Buffer is null");
        throw std::runtime_error("Buffer is null");
    }
    pipeline->bindInstanceBuffer(buffer);
    ENGINE_LOG_TRACE("OpenGLRHI::Instance buffer binded");
}

void OpenGLRHI::bindIndexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) {
    if(pipeline == nullptr) {
        ENGINE_LOG_CRITICAL("Pipeline is null");
        throw std::runtime_error("Pipeline is null");
    }
    if(buffer == nullptr) {
        ENGINE_LOG_CRITICAL("Buffer is null");
        throw std::runtime_error("Buffer is null");
    }
    pipeline->bindIndexBuffer(buffer);
    ENGINE_LOG_TRACE("OpenGLRHI::index buffer binded");
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
    (void)size;
    glUniformMatrix4fv(0 , 1, GL_FALSE, reinterpret_cast<const GLfloat*>(data));
}

/******** Draw ********/

void OpenGLRHI::draw(std::shared_ptr<Pipeline> pipeline, uint32_t count) {
    ENGINE_LOG_TRACE("OpenGLRHI::Drawing...");
    OpenGLPipeline* openGlPipeline = static_cast<OpenGLPipeline*>(pipeline.get());
    
    glBindVertexArray(openGlPipeline->getVertexArrayID());
    glDrawElementsInstanced(GL_TRIANGLES, openGlPipeline->getBindedNumberOfIndices(), GL_UNSIGNED_INT, nullptr, count);
    ENGINE_LOG_TRACE("OpenGLRHI::Drawed");
}
