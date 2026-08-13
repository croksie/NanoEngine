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
std::shared_ptr<Pipeline> OpenGLRHI::createPipeline() {
    ENGINE_LOG_TRACE("OpenGLRHI::Creating pipeline...");
    m_shader = compileShader();
    return std::make_shared<OpenGLPipeline>();
}


void OpenGLRHI::bindPipeline() {

    glUseProgram(m_shader); 
}

GLuint OpenGLRHI::compileShader()
{

    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    " void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);


    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}

void OpenGLRHI::draw(std::shared_ptr<Pipeline> pipeline) {
    ENGINE_LOG_TRACE("OpenGLRHI::Drawing...");
    OpenGLPipeline* openGlPipeline = static_cast<OpenGLPipeline*>(pipeline.get());
    
    glBindVertexArray(openGlPipeline->getPipelineID());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ENGINE_LOG_TRACE("OpenGLRHI::Drawed");
}
