#pragma once
#include "core/rhi/rhi.h"

#include <memory>


class Mesh {
public:
    Mesh() {}
    Mesh(std::shared_ptr<Buffer> vertexBuffer) : m_vertexBuffer(vertexBuffer) {}

    std::shared_ptr<Buffer> getVertexBuffer() { return m_vertexBuffer; }


private:
    std::shared_ptr<Buffer> m_vertexBuffer;
};