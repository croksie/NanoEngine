#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "math/mat4.h"

#include "bifrost/shader.h"

namespace midgard::platform {
    class Window;
}

namespace midgard::core {
    struct EngineConfig;
}

namespace midgard::bifrost {
    struct PipelineInfo;
    class Pipeline;
    struct BufferDesc;
    class Buffer;
    struct TextureDesc;
    class Texture;

struct InstanceData {
    math::Mat4 modelMatrix;
};

class Bifrost {
public:
    virtual ~Bifrost() = default;

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

    virtual void initialize(platform::Window* window, std::shared_ptr<core::EngineConfig> config) = 0;
    virtual void shutdown() = 0;
};

} // namespace midgard::bifrost

namespace midgard {
    using bifrost::Bifrost;
}
