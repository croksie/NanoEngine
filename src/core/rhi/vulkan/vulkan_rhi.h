#pragma once
#include "core/rhi/rhi.h"
#include "core/window.h"
#include "core/config.h"
#include "core/rhi/vulkan/vulkan_initializer.h"
#include "core/rhi/vulkan/vulkan_buffer.h"
#include "core/rhi/vulkan/vulkan_pipeline.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <string>

class VulkanPipeline;
class VulkanBuffer;

class VulkanRHI : public RHI {
public:
    void initialize(Window* window, std::shared_ptr<EngineConfig> config) override;
    void shutdown() override;

    void beginFrame() override;
    void endFrame() override;

    void clear() override;
    void draw(std::shared_ptr<Pipeline> pipeline) override;

    std::shared_ptr<Shader> createShader(ShaderType type, std::string source) override;

    std::shared_ptr<Buffer> createBuffer(float vertices[], size_t size) override;
    void bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) override;

    std::shared_ptr<Pipeline> createPipeline(PipelineInfo& info) override;
    void bindPipeline(Pipeline* pipeline) override;

    void setGlobalUniform(const void* data, size_t size) override;
    void setLocalUniform(const void* data, size_t size) override;


private:
    Window* m_window = nullptr;
    std::shared_ptr<EngineConfig> m_config;

    vulkan::VulkanContext m_ctx{};
    uint32_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;

    VulkanPipeline* m_currentPipeline = nullptr;

    void onWindowResize(int width, int heigth);
    bool m_framebufferResized = false;
    uint32_t m_width;
    uint32_t m_height;

    void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess, VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);
};
