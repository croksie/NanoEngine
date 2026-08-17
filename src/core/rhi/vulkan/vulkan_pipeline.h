#pragma once

#include "core/rhi/vulkan/vulkan_rhi.h"
#include "core/rhi/rhi_pipeline.h"
#include "core/rhi/vulkan/vulkan_shader.h"

#include <glm/glm.hpp>

#include <memory>

struct VulkanContext;

constexpr int VERTICLE_SIZE = 6 * sizeof(float);


class VulkanPipeline : public Pipeline {
public:
    VulkanPipeline() = delete;
    VulkanPipeline(PipelineInfo& info, VulkanContext& ctx);
    ~VulkanPipeline();

    /// @brief Set the commandBuffer to send command for bindVertexBuffer()
    /// @param cmdBuffer
    void setCmdBuffer(VkCommandBuffer cmdBuffer) { m_cmdBuffer = cmdBuffer; }

    /// @brief Send Command order to bind the vertexBuffer to the current pipeline.
    /// @brief setCmdBuffer() need to be call first. 
    /// @param vertexBuffer The vertex buffer to be bind.
    void bindVertexBuffer(std::shared_ptr<Buffer> vertexBuffer) override;


    VkPipeline getPipeline() { return m_pipeline; }
    VkPipelineLayout getPipelineLayout() { return m_pipelineLayout; }

    uint32_t getBindedNumberOfVerticles() { return m_numberOfVerticlesInBindedObject; }

private:
    VulkanContext* m_ctx;

    VkPipeline m_pipeline;
    VkPipelineLayout m_pipelineLayout;
    VkCommandBuffer m_cmdBuffer;

    uint32_t m_numberOfVerticlesInBindedObject = 0;

};