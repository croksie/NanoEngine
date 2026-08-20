#pragma once
#include "core/rhi/rhi.h"
#include "core/window.h"
#include "core/rhi/vulkan/vulkan_buffer.h"
#include "core/rhi/vulkan/vulkan_pipeline.h"

#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


#include <vector>
#include <array>
#include <iostream>
#include <memory>
#include <string>

class VulkanPipeline;
class VulkanBuffer;


const uint32_t WIDTH = 640;
const uint32_t HEIGHT = 480;

constexpr int MAX_FRAMES_IN_FLIGHT = 3;


struct FrameSyncObjects {
    VkSemaphore imageAvailableSemaphore; // When swapChain give image
    VkSemaphore renderFinishedSemaphore; // When GPU finish render
    VkFence inFlightFence;  // FIXME : Need to appart to use double buffering, othewise you can only use triple buffering
};

struct VulkanContext {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapchain;
    VkFormat swapchainFormat;
    VkFormat depthFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
};


class VulkanRHI : public RHI {
public:
    void initialize(Window* window) override;
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

    VulkanContext m_ctx{};
    VkCommandPool m_cmdPool;
    std::vector<VkCommandBuffer> m_cmdBuffers{MAX_FRAMES_IN_FLIGHT};
    std::vector<FrameSyncObjects> m_syncObjects{MAX_FRAMES_IN_FLIGHT};
    uint32_t m_currentFrame = 0;
    uint32_t m_imageIndex = 0;

    VkDescriptorPool m_descriptorPool;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::vector<VkDescriptorSet> m_descriptorSets{MAX_FRAMES_IN_FLIGHT};
    VkPipelineLayout m_pipelineLayout;
    VulkanPipeline* m_currentPipeline = nullptr;
    std::vector<std::unique_ptr<VulkanBuffer>> m_uniformBuffer;


    void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess);
};