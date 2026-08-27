#pragma once
#include <memory>

namespace midgard::bifrost {
    class Pipeline;
}

namespace midgard::resource {

class Material {
public:
    Material(std::shared_ptr<bifrost::Pipeline> pipeline) : m_pipeline(pipeline) {}
    std::shared_ptr<bifrost::Pipeline> getPipeline() const { return m_pipeline; }

private:
    std::shared_ptr<bifrost::Pipeline> m_pipeline;
};

} // namespace midgard::resource