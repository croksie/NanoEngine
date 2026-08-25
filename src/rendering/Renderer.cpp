#include "rendering/renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "utils/log.h"
#include "utils/file_utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

std::string assetFolder = "../../assets/";

std::string vertexShaderSource = fileUtils::readTextFile(assetFolder + "shaders/base.vert");
std::string fragmentShaderSource = fileUtils::readTextFile(assetFolder + "shaders/base.frag");


float vertices[] = {
    // Face Front
    -0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,         1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,         1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
    // Face Back
     0.5f, -0.5f, -0.5f,         0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,         0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
    // Face Left
    -0.5f, -0.5f, -0.5f,         0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,         0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,         0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,         0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
    // Face Right
     0.5f, -0.5f,  0.5f,         1.0f, 1.0f, 0.0f,     0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,         1.0f, 1.0f, 0.0f,     1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,         1.0f, 1.0f, 0.0f,     1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,         1.0f, 1.0f, 0.0f,     0.0f, 1.0f,
    // Face Up
    -0.5f,  0.5f,  0.5f,         0.0f, 1.0f, 1.0f,     0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,         0.0f, 1.0f, 1.0f,     1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 1.0f,     1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,         0.0f, 1.0f, 1.0f,     0.0f, 1.0f,
    // Face Down
    -0.5f, -0.5f, -0.5f,         1.0f, 0.0f, 1.0f,     0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,         1.0f, 0.0f, 1.0f,     1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 1.0f,     1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,         1.0f, 0.0f, 1.0f,     0.0f, 1.0f
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
    // Create texture
    int width = 0, height = 0, nrChannels = 0;
    unsigned char *data = stbi_load((assetFolder + "textures/wall.jpg").c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    if (!data) {
        ENGINE_LOG_ERROR("Renderer::createTestModel: Failed to load texture '{}'", assetFolder + "textures/wall.jpg");
    } else {
        ENGINE_LOG_INFO("Renderer::createTestModel: Loaded texture ({}x{}, channels: {})", width, height, nrChannels);
    }

    TextureDesc texDesc{};
    texDesc.width = static_cast<uint32_t>(width);
    texDesc.height = static_cast<uint32_t>(height);
    texDesc.format = TextureFormat::RGBA8_SRGB;
    texDesc.initialData = data;
    texDesc.size = width * height * 4;

    m_texture = m_rhi->createTexture(texDesc);

    if (data) {
        stbi_image_free(data);
    }

    // Create pipeline
    PipelineInfo info;
    info.vertexShader = m_rhi->createShader(ShaderType::VERTEX, vertexShaderSource);

    info.fragmentShader = m_rhi->createShader(ShaderType::FRAGMENT, fragmentShaderSource);
    info.useInstance = true;

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

    const float time = static_cast<float>(glfwGetTime());
    const glm::mat4 baseRotation = glm::rotate(
        glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        time * glm::radians(50.0f),
        glm::vec3(0.5f, 1.0f, 0.0f)
    );

    for (size_t i = 0; i < models.size(); ++i) {
        glm::mat4 modelMat = baseRotation;
        modelMat[3] = glm::vec4(static_cast<glm::vec3>(models[i].getPosition()), 1.0f);
        m_instances[i].modelMatrix = modelMat;
    }

    BufferDesc desc{};
    desc.size = m_instances.size() * sizeof(InstanceData);
    desc.type = BufferType::VERTEX;
    desc.initData = m_instances.data();

    m_instanceBuffer = m_rhi->createBuffer(desc);


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

    const float time = static_cast<float>(glfwGetTime());
    const glm::mat4 baseRotation = glm::rotate(
        glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        time * glm::radians(50.0f),
        glm::vec3(0.5f, 1.0f, 0.0f)
    );

    for (size_t i = 0; i < models.size(); ++i) {
        glm::mat4 modelMat = baseRotation;
        modelMat[3] = glm::vec4(static_cast<glm::vec3>(models[i].getPosition()), 1.0f);
        m_instances[i].modelMatrix = modelMat;
    }

    m_instanceBuffer->setData(m_instances.size() * sizeof(InstanceData), m_instances.data());



    //for (auto& model : models) {
        std::shared_ptr<Mesh> mesh = models[0].getMesh();
        std::shared_ptr<Material> material = models[0].getMaterial();

        m_rhi->bindPipeline(material->getPipeline().get());
        m_rhi->bindVertexBuffer(material->getPipeline(), mesh->getVertexBuffer());
        m_rhi->bindIndexBuffer(material->getPipeline(), mesh->getIndexBuffer());
        m_rhi->bindInstanceBuffer(material->getPipeline(), m_instanceBuffer);
        m_rhi->bindTexture(material->getPipeline(), m_texture, 1);
        m_rhi->draw(material->getPipeline(), 900);
    //}

    m_rhi->endFrame();
    ENGINE_LOG_TRACE("Render end");
}

void Renderer::shutdown()
{
    ENGINE_LOG_DEBUG("Renderer shutting down ...");
    models.clear();
    m_instanceBuffer = nullptr;
    m_texture = nullptr;
    m_rhi->shutdown(); // Ensure to have free all buffer and pipeline before
}