#pragma once
#include "core/rhi/rhi.h"

#include <memory>

class Material {
public:
    Material(std::shared_ptr<Pipeline> pipeline) : m_pipeline(pipeline) {}
    std::shared_ptr<Pipeline> getPipeline() const { return m_pipeline; }

private:
    std::shared_ptr<Pipeline> m_pipeline;
};