#pragma once
#include "rhi/rhi.h"
#include "rhi/rhi_pipeline.h"
#include "rhi/vulkan/vulkan_buffer.h"
#include "rhi/vulkan/vulkan_shader.h"
#include "rhi/vulkan/vulkan_texture.h"
#include "rhi/vulkan/vulkan_initializer.h"


#include <glm/glm.hpp>

#include <memory>

namespace vulkan {
    struct VulkanContext;
}

constexpr int VERTICLE_SIZE = 8 * sizeof(float);


class VulkanPipeline : public Pipeline {
public:
    VulkanPipeline() = delete;
    VulkanPipeline(PipelineInfo& info, vulkan::VulkanContext& ctx, VkPipelineLayout pipelineLayout);
    ~VulkanPipeline();

    void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override;
    void bindInstanceBuffer(std::shared_ptr<Buffer> instanceBuffer) override;
    void bindIndexBuffer(std::shared_ptr<Buffer> indexBuffer) override;
    void bindTexture(std::shared_ptr<Texture> texture, uint32_t slot = 0) override;


    VkPipeline getPipeline() { return m_pipeline; }
    VkPipelineLayout getPipelineLayout() { return m_pipelineLayout; }

    uint32_t getBindedNumberOfVerticles() { return m_numberOfVerticlesInBindedObject; }
    uint32_t getBindedNumberOfIndices() { return m_numberOfIndicesInBindedObject; }

private:
    vulkan::VulkanContext* m_ctx;

    bool m_useInstance = false;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;

    uint32_t m_numberOfVerticlesInBindedObject = 0;
    uint32_t m_numberOfIndicesInBindedObject = 0;

};