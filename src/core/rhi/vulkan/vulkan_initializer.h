#pragma once
#include "core/window.h"

#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class VulkanBuffer;

constexpr int MAX_FRAMES_IN_FLIGHT = 3;

const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;


namespace vulkan {

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
    uint32_t graphicsQueueFamily;

    // Swapchain
    VkSwapchainKHR swapchain;
    VkFormat swapchainFormat;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;

    // DepthBuffer
    VkFormat depthFormat = VK_FORMAT_UNDEFINED;
    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemories;
    std::vector<VkImageView> depthImageViews;

    // Fences / Semaphores
    std::vector<FrameSyncObjects> syncObjects{MAX_FRAMES_IN_FLIGHT};

    // CommandPool / CommandBuffer
    VkCommandPool cmdPool;
    std::vector<VkCommandBuffer> cmdBuffers{MAX_FRAMES_IN_FLIGHT};
    
    // Uniform Buffers
    std::vector<std::unique_ptr<VulkanBuffer>> uniformBuffers;

    // Descriptors
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets{MAX_FRAMES_IN_FLIGHT};

    // Pipeline Layout
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
};


// Initialize
VulkanContext createContext(Window* window);
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

// Utility
uint32_t findMemoryType(const VulkanContext& ctx, uint32_t typeFilter, VkMemoryPropertyFlags properties);

}
