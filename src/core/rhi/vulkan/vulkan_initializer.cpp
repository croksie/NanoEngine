#include "core/rhi/vulkan/vulkan_initializer.h"
#include "core/rhi/vulkan/vulkan_buffer.h"

#include <glm/glm.hpp>
#include "utils/log.h"

vulkan::VulkanContext vulkan::createContext(Window *window) {
    window->initializeWindow(WIDTH, HEIGHT, "NanoEngine");

    vulkan::VulkanContext ctx;

    ENGINE_LOG_TRACE("VulkanRHI::Creating instance...");
    // Use vkboostrap to initialize vulkan
    vkb::InstanceBuilder instBuilder;
    auto instResult = instBuilder
        .set_app_name("NanoEngine")
        #ifdef DEBUG
            .request_validation_layers(true) 
            .use_default_debug_messenger()
        #endif
        .require_api_version(1, 3, 0) // Force Vulkan 1.3 in order to use Dynamic Rendering
        .build();

    if (!instResult) {
        throw std::runtime_error(instResult.error().message());
    }
    vkb::Instance vkbInst = instResult.value();
    ctx.instance = vkbInst.instance;
    ctx.debugMessenger = vkbInst.debug_messenger;

    // Create surface
    glfwCreateWindowSurface(ctx.instance, static_cast<GLFWwindow*>(window->getNativeHandle()), nullptr, &ctx.surface);

    // Use Vulkan 1.3 features (dynamicRendering, etc)
    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;    // Suppress RenderPass / Framebuffers
    features13.synchronization2 = VK_TRUE;    // Simplified Sync

    ENGINE_LOG_TRACE("VulkanRHI::Choosing physical device...");
    // Select Physical Device
    vkb::PhysicalDeviceSelector selector{ vkbInst };
    auto physResult = selector
        .set_surface(ctx.surface)
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
        .select();

    if (!physResult) {
        throw std::runtime_error(physResult.error().message());
    }
    vkb::PhysicalDevice vkbPhysDevice = physResult.value();
    ctx.physicalDevice = vkbPhysDevice.physical_device;
    // TODO : Add the possibility to choose the device if multiple device detected

    ENGINE_LOG_TRACE("VulkanRHI::Creating logical device and queue...");
    // Create logical device and queue
    vkb::DeviceBuilder devBuilder{ vkbPhysDevice };
    auto devResult = devBuilder.build();
    if (!devResult) {
        throw std::runtime_error(devResult.error().message());
    }
    vkb::Device vkbDevice = devResult.value();
    ctx.device = vkbDevice.device;

    ctx.graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    ctx.graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
    ctx.presentQueue = vkbDevice.get_queue(vkb::QueueType::present).value();

    return ctx;
}

// Swapchain
void vulkan::createSwapchain(VulkanContext &ctx, uint32_t width, uint32_t height) {

    ENGINE_LOG_TRACE("VulkanRHI::Creating swapchain...");
    vkb::SwapchainBuilder scBuilder{ ctx.physicalDevice, ctx.device, ctx.surface };
    auto scResult = scBuilder
        .set_desired_extent(width, height)
        .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR) // Use VK_PRESENT_MODE_FIFO_KHR for v-sync and VK_PRESENT_MODE_IMMEDIATE_KHR for unlimited
        // TODO : Add the possibility to change present mode at runtime or make an artificial v-sync
        .build();

    if (!scResult) {
        throw std::runtime_error(scResult.error().message());
    }
    vkb::Swapchain vkbSwapchain = scResult.value();
    
    ctx.swapchain = vkbSwapchain.swapchain;
    ctx.swapchainFormat = vkbSwapchain.image_format;
    ctx.swapchainImages = vkbSwapchain.get_images().value();
    ctx.swapchainImageViews = vkbSwapchain.get_image_views().value();
}

// Depth Buffer
void vulkan::createDepthBuffer(VulkanContext &ctx, uint32_t width, uint32_t height) {
    // DepthFormat
    ENGINE_LOG_TRACE("VulkanRHI::Checking depth format...");
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(ctx.physicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            ctx.depthFormat = format;
            break;
        }
    }
    if(ctx.depthFormat == VK_FORMAT_UNDEFINED ) {
        ENGINE_LOG_CRITICAL("VulkanRHI::No compatible depth format found");
        throw std::runtime_error("No compatible depth format found");
    }

    // DepthImages
    ENGINE_LOG_TRACE("VulkanRHI::Creating depth resources...");
    ctx.depthImages.resize(MAX_FRAMES_IN_FLIGHT);
    ctx.depthImageMemories.resize(MAX_FRAMES_IN_FLIGHT);
    ctx.depthImageViews.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkImageCreateInfo depthImageInfo{};
        depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
        depthImageInfo.extent.width = static_cast<uint32_t>(width);
        depthImageInfo.extent.height = static_cast<uint32_t>(height);
        depthImageInfo.extent.depth = 1;
        depthImageInfo.mipLevels = 1;
        depthImageInfo.arrayLayers = 1;
        depthImageInfo.format = ctx.depthFormat;
        depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        depthImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(ctx.device, &depthImageInfo, nullptr, &ctx.depthImages[i]) != VK_SUCCESS) {
            ENGINE_LOG_CRITICAL("VulkanRHI::Failed to create depth image");
            throw std::runtime_error("Failed to create depth image");
        }

        VkMemoryRequirements depthMemReqs;
        vkGetImageMemoryRequirements(ctx.device, ctx.depthImages[i], &depthMemReqs);

        VkMemoryAllocateInfo depthAllocInfo{};
        depthAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        depthAllocInfo.allocationSize = depthMemReqs.size;
        depthAllocInfo.memoryTypeIndex = findMemoryType(ctx, depthMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(ctx.device, &depthAllocInfo, nullptr, &ctx.depthImageMemories[i]) != VK_SUCCESS) {
            ENGINE_LOG_CRITICAL("VulkanRHI::Failed to allocate depth image memory");
            throw std::runtime_error("Failed to allocate depth image memory");
        }

        vkBindImageMemory(ctx.device, ctx.depthImages[i], ctx.depthImageMemories[i], 0);
        // Depth images views
        VkImageViewCreateInfo depthViewInfo{};
        depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        depthViewInfo.image = ctx.depthImages[i];
        depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        depthViewInfo.format = ctx.depthFormat;
        depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        depthViewInfo.subresourceRange.baseMipLevel = 0;
        depthViewInfo.subresourceRange.levelCount = 1;
        depthViewInfo.subresourceRange.baseArrayLayer = 0;
        depthViewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(ctx.device, &depthViewInfo, nullptr, &ctx.depthImageViews[i]) != VK_SUCCESS) {
            ENGINE_LOG_CRITICAL("VulkanRHI::Failed to create depth image view");
            throw std::runtime_error("Failed to create depth image view");
        }
    }
}

void vulkan::createFenceAndSemaphore(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating fence and semaphore...");
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.syncObjects[i].imageAvailableSemaphore);
        vkCreateSemaphore(ctx.device, &semaphoreInfo, nullptr, &ctx.syncObjects[i].renderFinishedSemaphore);
        vkCreateFence(ctx.device, &fenceInfo, nullptr, &ctx.syncObjects[i].inFlightFence);
    }
}

void vulkan::createCommandPool(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating command pool and command buffer...");
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = ctx.graphicsQueueFamily;

    vkCreateCommandPool(ctx.device, &poolInfo, nullptr, &ctx.cmdPool);
}

void vulkan::createCommandBuffer(VulkanContext &ctx) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = ctx.cmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT; 

    if (vkAllocateCommandBuffers(ctx.device, &allocInfo, ctx.cmdBuffers.data()) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to allocate command buffers");
        throw std::runtime_error("Failed to allocate command buffers");
    }
}

void vulkan::createUniformBuffers(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating uniform buffer...");
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        ctx.uniformBuffers.emplace_back(std::make_unique<VulkanBuffer>(2048, nullptr, ctx, VulkanBufferType::UNIFORM)); // FIXME Change the buffer size
    }
}

void vulkan::createDescriptorSetLayout(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating descriptor set layout...");
    VkDescriptorSetLayoutBinding globalLayoutBinding{};
    globalLayoutBinding.binding = 0;
    globalLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    globalLayoutBinding.descriptorCount = 1;
    globalLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Accessible dans le vertex shader
    globalLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.bindingCount = 1;
    descriptorLayoutInfo.pBindings = &globalLayoutBinding;

    if (vkCreateDescriptorSetLayout(ctx.device, &descriptorLayoutInfo, nullptr, &ctx.descriptorSetLayout) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to create descriptor set layout");
        throw std::runtime_error("Failed to create descriptor set layout");
    }
}

void vulkan::createDescriptorPool(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating descriptor pool...");
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &poolSize;
    descriptorPoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(ctx.device, &descriptorPoolInfo, nullptr, &ctx.descriptorPool) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to create descriptor pool");
        throw std::runtime_error("Failed to create descriptor pool");
    }
}

void vulkan::createDescriptorSets(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Allocating descriptor sets...");
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, ctx.descriptorSetLayout);

    VkDescriptorSetAllocateInfo descriptorAllocInfo{};
    descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorAllocInfo.descriptorPool = ctx.descriptorPool;
    descriptorAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    descriptorAllocInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(ctx.device, &descriptorAllocInfo, ctx.descriptorSets.data()) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to allocate descriptor sets");
        throw std::runtime_error("Failed to allocate descriptor sets");
    }

    ENGINE_LOG_TRACE("VulkanRHI::Writing descriptor sets...");
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = *ctx.uniformBuffers[i]->getBufferHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = ctx.descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(ctx.device, 1, &descriptorWrite, 0, nullptr);
    }
}

void vulkan::createPipelineLayout(VulkanContext &ctx) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating pipeline layout...");
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4); // TODO : Maybe use an abstraction

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &ctx.descriptorSetLayout;
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(ctx.device, &layoutInfo, nullptr, &ctx.pipelineLayout) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to create pipeline layout");
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

/* ------------ Cleanup ------------ */


void vulkan::cleanupSwapchain(VulkanContext &ctx) {
    vkDeviceWaitIdle(ctx.device);

    vkDestroySwapchainKHR(ctx.device, ctx.swapchain, nullptr);
    //destroy swapchain resources
    for (int i = 0; i < ctx.swapchainImageViews.size(); i++) {
        vkDestroyImageView(ctx.device, ctx.swapchainImageViews[i], nullptr);
    }

}






/* ------------ Utility ------------ */

uint32_t vulkan::findMemoryType(const VulkanContext& ctx, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(ctx.physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    ENGINE_LOG_CRITICAL("VulkanRHI::Failed to find suitable memory type");
    throw std::runtime_error("Failed to find suitable memory type");
}
