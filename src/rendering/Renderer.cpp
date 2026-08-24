#include "rendering/renderer.h"

#include "utils/log.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>


std::string vulkaVertexShaderSource = R"(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

layout (location = 0) out vec3 ourColor;

layout (std140, binding = 0) uniform GlobalData {
    mat4 view;
    mat4 projection;
} u_Global;

layout (push_constant) uniform PushConstants {
    mat4 model;
} u_Object;

void main() {
    gl_Position = u_Global.projection * u_Global.view * u_Object.model * vec4(aPos, 1.0);
    ourColor = aCol;
}
)";

std::string openglVertexShaderSource = R"(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

layout (location = 0) out vec3 ourColor;

layout (std140, binding = 0) uniform GlobalData {
    mat4 view;
    mat4 projection;
} u_Global;

layout (location = 0) uniform mat4 u_Model;

void main() {
    gl_Position = u_Global.projection * u_Global.view * u_Model * vec4(aPos, 1.0);
    ourColor = aCol;
}
)";
std::string fragmentShaderSource = R"(
#version 450 core
layout (location = 0) in vec3 ourColor;
layout (location = 0) out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0f);
})";


float vertices[] = {
    // Face Front
    -0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,         1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,         1.0f, 0.0f, 0.0f,
    // Face Back
     0.5f, -0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 0.0f,
    // Face Left
    -0.5f, -0.5f, -0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,         0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,         0.0f, 0.0f, 1.0f,
    // Face Right
     0.5f, -0.5f,  0.5f,         1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,         1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,         1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,         1.0f, 1.0f, 0.0f,
    // Face Up
    -0.5f,  0.5f,  0.5f,         0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,         0.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 1.0f,
    // Face Down
    -0.5f, -0.5f, -0.5f,         1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,         1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 1.0f
};

uint32_t indices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,
    // Back
    4, 5, 6,
    6, 7, 4,
    // Left
    8, 9, 10,
    10, 11, 8,
    // Right
    12, 13, 14,
    14, 15, 12,
    // Up
    16, 17, 18,
    18, 19, 16,
    // Down
    20, 21, 22,
    22, 23, 20
};




void Renderer::createTestModel(){
    // Create pipeline
    PipelineInfo info;
    if(m_config->api == GraphicsAPI::OpenGL) {
        info.vertexShader = m_rhi->createShader(ShaderType::VERTEX, openglVertexShaderSource);
    }
    else {
        info.vertexShader = m_rhi->createShader(ShaderType::VERTEX, vulkaVertexShaderSource);
    }
    info.fragmentShader = m_rhi->createShader(ShaderType::FRAGMENT, fragmentShaderSource);

    Material mat = Material(m_rhi->createPipeline(info));
    ENGINE_LOG_DEBUG("Is pipeline valid ? : {}", mat.getPipeline() != nullptr ? "true" : "false");

    BufferDesc vertexBufferDesc{};
    vertexBufferDesc.initData = vertices;
    vertexBufferDesc.size = sizeof(vertices);
    vertexBufferDesc.type = BufferType::VERTEX;
    std::shared_ptr<Buffer> vertexBuffer = m_rhi->createBuffer(vertexBufferDesc);

    BufferDesc indexBufferDesc{};
    indexBufferDesc.initData = indices;
    indexBufferDesc.size = sizeof(indices);
    indexBufferDesc.type = BufferType::INDEX;
    std::shared_ptr<Buffer> indexBuffer = m_rhi->createBuffer(indexBufferDesc);

    Mesh mesh = Mesh(vertexBuffer, indexBuffer);

    for(int i = -15; i<15 ; ++i) {
        for(int j = -15; j<15 ; ++j) {
            Model model = Model(std::make_shared<Mesh>(mesh), std::make_shared<Material>(mat));
            model.setPosition(Vec3(2.0f*i, 2.0f*j, -15.0f));
            models.push_back(model);
        }
    }
}

void Renderer::initialize(Window* window, std::shared_ptr<EngineConfig> config) {
    ENGINE_LOG_INFO("Renderer initializing ...");
    m_config = config;
    // Init RHI
    if(m_config->api == GraphicsAPI::OpenGL) {
        m_rhi = std::make_unique<OpenGLRHI>();
    } else {
        m_rhi = std::make_unique<VulkanRHI>();
    }
    m_rhi->initialize(window, m_config);

    createTestModel();

    ENGINE_LOG_INFO("Renderer initialized");
}

void Renderer::render() {
    // View matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); 
    //Projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    glm::mat4 matrices[2] = {view, projection};
    const void* data = matrices;


    ENGINE_LOG_TRACE("Render start");
    m_rhi->beginFrame();
    m_rhi->clear();
    m_rhi->setGlobalUniform(data, sizeof(matrices));




    for (auto& model : models) {
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, (glm::vec3)model.getPosition());
        modelMat = glm::rotate(modelMat, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
        modelMat = glm::rotate(modelMat, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        
        std::shared_ptr<Mesh> mesh = model.getMesh();
        std::shared_ptr<Material> material = model.getMaterial();

        m_rhi->bindPipeline(material->getPipeline().get());
        m_rhi->bindVertexBuffer(material->getPipeline(), mesh->getVertexBuffer());
        m_rhi->bindIndexBuffer(material->getPipeline(), mesh->getIndexBuffer());
        m_rhi->setLocalUniform(glm::value_ptr(modelMat), sizeof(modelMat));
        m_rhi->draw(material->getPipeline());
    }

    m_rhi->endFrame();
    ENGINE_LOG_TRACE("Render end");
}

void Renderer::shutdown()
{
    ENGINE_LOG_DEBUG("Renderer shutting down ...");
    models.clear();
    m_rhi->shutdown();

}