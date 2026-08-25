#pragma once
#include "rhi/rhi_buffer.h"
#include "rhi/rhi_pipeline.h"
#include "rhi/rhi_shader.h"
#include "rhi/rhi_texture.h"

#include "core/window.h"
#include "core/config.h"

#include "glm/glm.hpp"

#include <stddef.h>

#include <memory>
#include <string>

struct InstanceData {
    glm::mat4 modelMatrix;
};

class RHI{

public:
    virtual ~RHI() = default;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void clear() = 0;
    virtual void draw(std::shared_ptr<Pipeline> pipeline, uint32_t count = 1) = 0;

    virtual std::shared_ptr<Shader> createShader(ShaderType type, std::string source) = 0;
    virtual std::shared_ptr<Pipeline> createPipeline(PipelineInfo& info) = 0;
    virtual std::shared_ptr<Buffer> createBuffer(BufferDesc& desc) = 0;
    virtual std::shared_ptr<Texture> createTexture(const TextureDesc& desc) = 0;


    virtual void bindPipeline(Pipeline* pipeline) = 0;
    virtual void bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) = 0;
    virtual void bindInstanceBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) = 0;
    virtual void bindIndexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) = 0;
    virtual void bindTexture(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Texture> texture, uint32_t slot = 0) = 0;

    virtual void setGlobalUniform(const void* data, size_t size) = 0;
    virtual void setLocalUniform(const void* data, size_t size) = 0;

    virtual void initialize(Window* window, std::shared_ptr<EngineConfig> config) = 0;
    virtual void shutdown() = 0;

    

};