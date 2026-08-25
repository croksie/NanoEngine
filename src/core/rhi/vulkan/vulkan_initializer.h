#pragma once
#include "core/window.h"
#include "core/config.h"
#include "core/rhi/vulkan/vulkan_buffer.h"

#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class VulkanBuffer;

namespace vulkan {

struct FrameSyncObjects {
    VkSemaphore imageAvailableSemaphore; // When swapChain give image
    VkSemaphore renderFinishedSemaphore; // When GPU finish render
    VkFence inFlightFence;  // FIXME : Need to appart to use double buffering, othewise you can only use triple buffering
};

struct VulkanContext {
    VulkanContext(uint32_t maxFramesInFlight = 3) 
        : maxFramesInFlight(maxFramesInFlight),
          syncObjects(maxFramesInFlight),
          cmdBuffers(maxFramesInFlight),
          descriptorSets(maxFramesInFlight) {}

    uint32_t maxFramesInFlight = 3;
    uint32_t currentFrame = 0;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = 0;

    // Swapchain
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    // DepthBuffer
    VkFormat depthFormat = VK_FORMAT_UNDEFINED;
    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemories;
    std::vector<VkImageView> depthImageViews;

    // Fences / Semaphores
    std::vector<FrameSyncObjects> syncObjects;

    // CommandPool / CommandBuffer
    VkCommandPool cmdPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> cmdBuffers;
    
    // Uniform Buffers
    std::vector<std::unique_ptr<VulkanBuffer>> uniformBuffers;

    // Descriptors
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

    // Pipeline Layout
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};


// Initialize
VulkanContext createContext(Window* window, std::shared_ptr<EngineConfig> config);
void createSwapchain(VulkanContext& ctx, uint32_t width, uint32_t height);
void createDepthBuffer(VulkanContext& ctx, uint32_t width, uint32_t height);
void createFenceAndSemaphore(VulkanContext& ctx);
void createCommandPool(VulkanContext& ctx);
void createCommandBuffer(VulkanContext& ctx);
void createUniformBuffers(VulkanContext& ctx);
void createDescriptorSetLayout(VulkanContext& ctx);
void createDescriptorPool(VulkanContext& ctx);
void createDescriptorSets(VulkanContext& ctx);
void createPipelineLayout(VulkanContext& ctx);

// CleanUp
void cleanupSwapchain(VulkanContext& ctx);
void cleanupDepthBuffer(VulkanContext& ctx);

// Utility
uint32_t findMemoryType(const VulkanContext& ctx, uint32_t typeFilter, VkMemoryPropertyFlags properties);
VkCommandBuffer beginSingleTimeCommands(const VulkanContext& ctx);
void endSingleTimeCommands(const VulkanContext& ctx, VkCommandBuffer commandBuffer);
void transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
                           VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,
                           VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess,
                           VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

}
