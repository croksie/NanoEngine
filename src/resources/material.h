#pragma once
#include <memory>

#include "resources/default.h"

namespace midgard::bifrost {
    class Pipeline;
}

namespace midgard::resource {

class Material {
public:
    Material(std::shared_ptr<bifrost::Pipeline> pipeline, std::shared_ptr<bifrost::Texture> texture) 
        : m_pipeline(pipeline), m_texture(texture) {}

    Material(std::shared_ptr<bifrost::Pipeline> pipeline) 
        : m_pipeline(pipeline), m_texture(DefaultResources::getDefaultTexture()) {}

    std::shared_ptr<bifrost::Pipeline> getPipeline() const { return m_pipeline; }
    std::shared_ptr<bifrost::Texture> getTexture() const { return m_texture; }

private:
    std::shared_ptr<bifrost::Pipeline> m_pipeline;
    std::shared_ptr<bifrost::Texture> m_texture;
};

} // namespace midgard::resource