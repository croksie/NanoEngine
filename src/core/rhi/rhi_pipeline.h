#pragma once
#include "core/rhi/rhi_buffer.h"

#include <memory>

class Pipeline {
public:
    virtual ~Pipeline() = default;

    virtual void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) = 0;

};