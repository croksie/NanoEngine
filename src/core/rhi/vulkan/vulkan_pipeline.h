#pragma once
#include "core/rhi/rhi_pipeline.h"
#include "core/rhi/vulkan/vulkan_shader.h"

#include <glm/glm.hpp>

#include <memory>

namespace vulkan {
    struct VulkanContext;
}

constexpr int VERTICLE_SIZE = 6 * sizeof(float);


class VulkanPipeline : public Pipeline {
public:
    VulkanPipeline() = delete;
    VulkanPipeline(PipelineInfo& info, vulkan::VulkanContext& ctx, VkPipelineLayout pipelineLayout);
    ~VulkanPipeline();


    void setCmdBuffer(VkCommandBuffer cmdBuffer) { m_cmdBuffer = cmdBuffer; }

    void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override;
    void bindIndexBuffer(std::shared_ptr<Buffer> indexBuffer) override;


    VkPipeline getPipeline() { return m_pipeline; }
    VkPipelineLayout getPipelineLayout() { return m_pipelineLayout; }

    uint32_t getBindedNumberOfVerticles() { return m_numberOfVerticlesInBindedObject; }
    uint32_t getBindedNumberOfIndices() { return m_numberOfIndicesInBindedObject; }

private:
    vulkan::VulkanContext* m_ctx;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;
    VkCommandBuffer m_cmdBuffer;

    uint32_t m_numberOfVerticlesInBindedObject = 0;
    uint32_t m_numberOfIndicesInBindedObject = 0;

};