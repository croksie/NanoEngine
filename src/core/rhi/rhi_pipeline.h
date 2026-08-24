#pragma once
#include "core/rhi/rhi_buffer.h"

#include "core/rhi/opengl/opengl_shader.h"

#include <memory>


struct PipelineInfo {
    std::shared_ptr<Shader> vertexShader;
    std::shared_ptr<Shader> fragmentShader;
};

class Pipeline {
public:
    virtual ~Pipeline() = default;

    virtual void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) = 0;
    virtual void bindIndexBuffer(std::shared_ptr<Buffer> indexBuffer) = 0;

};