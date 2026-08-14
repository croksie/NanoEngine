#include "rendering/renderer.h"

#include "utils/log.h"


#include <string>
#include "Renderer.h"

std::string vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
out vec3 ourColor;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aCol;
})";

std::string fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0f);
})";

float vertices[] = {
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

void Renderer::createTestModel(){
    // Create pipeline
    PipelineInfo info;
    info.vertexShader = std::make_shared<OpenGLShader>(ShaderType::VERTEX, vertexShaderSource);
    info.fragmentShader = std::make_shared<OpenGLShader>(ShaderType::FRAGMENT, fragmentShaderSource);

    Material mat = Material(m_rhi->createPipeline(info));
    ENGINE_LOG_DEBUG("Is pipeline valid ? : {}", mat.getPipeline() != nullptr ? "true" : "false");

    std::shared_ptr<Buffer> buffer = m_rhi->createBuffer(vertices, sizeof(vertices));
    Mesh mesh = Mesh(buffer);

    Model model = Model(std::make_shared<Mesh>(mesh), std::make_shared<Material>(mat));
    models.push_back(model);

}

void Renderer::initialize(Window* window) {
    ENGINE_LOG_INFO("Renderer initializing ...");


    // Init RHI
    m_rhi = std::make_unique<OpenGLRHI>();
    m_rhi->initialize(window);

    createTestModel();

    ENGINE_LOG_INFO("Renderer initialized");
}

void Renderer::render() {
    ENGINE_LOG_DEBUG("Render start");
    m_rhi->beginFrame();
    m_rhi->clear();

    for (auto& model : models) {
        std::shared_ptr<Mesh> mesh = model.getMesh();
        std::shared_ptr<Material> material = model.getMaterial();

        m_rhi->bindPipeline(material->getPipeline().get());
        m_rhi->bindVertexBuffer(material->getPipeline(), mesh->getVertexBuffer());
        m_rhi->draw(material->getPipeline());
    }

    m_rhi->endFrame();
    ENGINE_LOG_DEBUG("Render end");
}