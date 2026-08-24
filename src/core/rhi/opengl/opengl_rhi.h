#pragma once
#include "core/rhi/rhi.h"
#include "core/window.h"
#include "core/config.h"

#include "core/rhi/opengl/opengl_buffer.h"
#include "core/rhi/opengl/opengl_pipeline.h"

#include <memory>
#include <string>

class OpenGLRHI : public RHI {

public:
    void initialize(Window* window, std::shared_ptr<EngineConfig> config) override;
    void shutdown() override {}

    void beginFrame() override;
    void endFrame() override;

    void clear() override;
    void draw(std::shared_ptr<Pipeline> pipeline) override;

    std::shared_ptr<Shader> createShader(ShaderType type, std::string source) override;

    std::shared_ptr<Buffer> createBuffer(float vertices[], size_t size) override;
    void bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) override;

    std::shared_ptr<Pipeline> createPipeline(PipelineInfo& info) override;
    void bindPipeline(Pipeline* pipeline) override;

    void setGlobalUniform(const void* data, size_t size) override;
    void setLocalUniform(const void* data, size_t size) override;

private:
    Window* m_window = nullptr;
    std::shared_ptr<EngineConfig> m_config;

    std::unique_ptr<OpenGLBuffer> m_uniformBuffer;

    void onWindowResize(int width, int heigth);
};