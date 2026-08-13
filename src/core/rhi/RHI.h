#pragma once
#include "core/window.h"
#include "core/rhi/rhi_buffer.h"
#include "core/rhi/rhi_pipeline.h"


#include <memory>

class RHI{

public:
    virtual ~RHI() = default;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void clear() = 0;
    virtual void draw(std::shared_ptr<Pipeline> pipeline) = 0;

    virtual std::shared_ptr<Buffer> createBuffer(float vertices[], size_t size) = 0;
    virtual void bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) = 0;

    virtual std::shared_ptr<Pipeline> createPipeline() = 0;
    virtual void bindPipeline() = 0;

    virtual void initialize(Window* window) = 0;
    virtual void shutdown() = 0;

    

};