#pragma once
#include "rhi/rhi.h"
#include "rhi/opengl/opengl_buffer.h"
#include "rhi/opengl/opengl_pipeline.h"
#include "rhi/opengl/opengl_texture.h"
#include "core/window.h"
#include "core/config.h"

#include <memory>
#include <string>

class OpenGLRHI : public RHI {

public:
    void initialize(Window* window, std::shared_ptr<EngineConfig> config) override;
    void shutdown() override {}

    void beginFrame() override;
    void endFrame() override;

    void clear() override;
    void draw(std::shared_ptr<Pipeline> pipeline, uint32_t count) override;

    std::shared_ptr<Shader> createShader(ShaderType type, std::string source) override;
    std::shared_ptr<Pipeline> createPipeline(PipelineInfo& info) override;
    std::shared_ptr<Buffer> createBuffer(BufferDesc& desc) override;
    std::shared_ptr<Texture> createTexture(const TextureDesc& desc) override;

    void bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) override;
    void bindInstanceBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) override;
    void bindIndexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) override;
    void bindTexture(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Texture> texture, uint32_t slot = 0) override;
    void bindPipeline(Pipeline* pipeline) override;

    void setGlobalUniform(const void* data, size_t size) override;
    void setLocalUniform(const void* data, size_t size) override;

private:
    Window* m_window = nullptr;
    std::shared_ptr<EngineConfig> m_config;

    std::unique_ptr<OpenGLBuffer> m_uniformBuffer;

    void onWindowResize(int width, int heigth);
};