#include "core/rhi/vulkan/vulkan_rhi.h"

#include <utils/log.h>
#include "vulkan_rhi.h"


void VulkanRHI::initialize(Window *window) {
    m_window = window;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window->initializeWindow(WIDTH, HEIGHT, "NanoEngine");

    ENGINE_LOG_TRACE("VulkanRHI::Creating instance...");
    // Use vkboostrap to initialize vulkan
    vkb::InstanceBuilder instBuilder;
    auto instResult = instBuilder
        .set_app_name("NanoEngine")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0) // Force Vulkan 1.3 in order to use Dynamic Rendering
        .build();

    if (!instResult) {
        throw std::runtime_error(instResult.error().message());
    }
    vkb::Instance vkbInst = instResult.value();
    m_ctx.instance = vkbInst.instance;
    m_ctx.debugMessenger = vkbInst.debug_messenger;

    // Create surface
    glfwCreateWindowSurface(m_ctx.instance, static_cast<GLFWwindow*>(m_window->getNativeHandle()), nullptr, &m_ctx.surface);

    // Use Vulkan 1.3 features (dynamicRendering, etc)
    VkPhysicalDeviceVulkan13Features features13{};
    features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    features13.dynamicRendering = VK_TRUE;    // Suppress RenderPass / Framebuffers
    features13.synchronization2 = VK_TRUE;    // Simplified Sync

    ENGINE_LOG_TRACE("VulkanRHI::Choosing physical device...");
    // Select Physical Device
    vkb::PhysicalDeviceSelector selector{ vkbInst };
    auto physResult = selector
        .set_surface(m_ctx.surface)
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
        .select();

    if (!physResult) {
        throw std::runtime_error(physResult.error().message());
    }
    vkb::PhysicalDevice vkbPhysDevice = physResult.value();
    m_ctx.physicalDevice = vkbPhysDevice.physical_device;
    // TODO : Add the possibility to choose the device if multiple device detected

    ENGINE_LOG_TRACE("VulkanRHI::Creating logical device and queue...");
    // Create logical device and queue
    vkb::DeviceBuilder devBuilder{ vkbPhysDevice };
    auto devResult = devBuilder.build();
    if (!devResult) {
        throw std::runtime_error(devResult.error().message());
    }
    vkb::Device vkbDevice = devResult.value();
    m_ctx.device = vkbDevice.device;

    m_ctx.graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    m_ctx.presentQueue = vkbDevice.get_queue(vkb::QueueType::present).value();

    /// Create swapchain
    int width, height;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_window->getNativeHandle()), &width, &height);

    ENGINE_LOG_TRACE("VulkanRHI::Creating swapchain...");
    vkb::SwapchainBuilder scBuilder{ vkbDevice };
    auto scResult = scBuilder
        .set_desired_extent(width, height)
        .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR) // V-Sync Enable    VK_PRESENT_MODE_IMMEDIATE_KHR to unlimited
        // TODO : Add the possibility to change present mode at runtime or make an artificial v-sync
        .build();

    if (!scResult) {
        throw std::runtime_error(scResult.error().message());
    }
    vkb::Swapchain vkbSwapchain = scResult.value();
    
    m_ctx.swapchain = vkbSwapchain.swapchain;
    m_ctx.swapchainFormat = vkbSwapchain.image_format;
    m_ctx.swapchainImages = vkbSwapchain.get_images().value();
    m_ctx.swapchainImageViews = vkbSwapchain.get_image_views().value();

    // DepthFormat
    ENGINE_LOG_TRACE("VulkanRHI::Checking depth format...");
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_ctx.physicalDevice, format, &props);

        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            m_ctx.depthFormat = format;
        }
    }
    if(m_ctx.depthFormat == VK_FORMAT_UNDEFINED ) {
        ENGINE_LOG_CRITICAL("VulkanRHI::No compatible depth format found");
        throw std::runtime_error("No compatible depth format found");
    }

    // Create Fence and semaphore
    ENGINE_LOG_TRACE("VulkanRHI::Creating fence and semaphore...");
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(m_ctx.device, &semaphoreInfo, nullptr, &m_syncObjects[i].imageAvailableSemaphore);
        vkCreateSemaphore(m_ctx.device, &semaphoreInfo, nullptr, &m_syncObjects[i].renderFinishedSemaphore);
        vkCreateFence(m_ctx.device, &fenceInfo, nullptr, &m_syncObjects[i].inFlightFence);
    }

    // Create Command Pool and Command Buffer
    ENGINE_LOG_TRACE("VulkanRHI::Creating command pool and command buffer...");
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    vkCreateCommandPool(m_ctx.device, &poolInfo, nullptr, &m_cmdPool);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_cmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT; 

    vkAllocateCommandBuffers(m_ctx.device, &allocInfo, m_cmdBuffers.data());

    // Create uniform buffer
    ENGINE_LOG_TRACE("VulkanRHI::Creating uniform buffer...");
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_uniformBuffer.emplace_back(std::make_unique<VulkanBuffer>(2048, nullptr, m_ctx, VulkanBufferType::UNIFORM)); // FIXME Change the buffer size
    }

    // Create Descriptor Set Layout
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

    vkCreateDescriptorSetLayout(m_ctx.device, &descriptorLayoutInfo, nullptr, &m_descriptorSetLayout);

    // Create Descriptor Pool
    ENGINE_LOG_TRACE("VulkanRHI::Creating descriptor pool...");
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &poolSize;
    descriptorPoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    vkCreateDescriptorPool(m_ctx.device, &descriptorPoolInfo, nullptr, &m_descriptorPool);

    // Allocate Descriptor Sets
    ENGINE_LOG_TRACE("VulkanRHI::Allocating descriptor sets...");
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);

    VkDescriptorSetAllocateInfo descriptorAllocInfo{};
    descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorAllocInfo.descriptorPool = m_descriptorPool;
    descriptorAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    descriptorAllocInfo.pSetLayouts = layouts.data();

    vkAllocateDescriptorSets(m_ctx.device, &descriptorAllocInfo, m_descriptorSets.data());

    // Write Descriptor Sets
    ENGINE_LOG_TRACE("VulkanRHI::Writing descriptor sets...");
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = *m_uniformBuffer[i]->getBufferHandle();
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_ctx.device, 1, &descriptorWrite, 0, nullptr);
    }

    // Create Pipeline Layout
    ENGINE_LOG_TRACE("VulkanRHI::Creating pipeline layout...");
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4); // TODO : Maybe use an abstraction

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &m_descriptorSetLayout;
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pPushConstantRanges = &pushConstantRange;

    vkCreatePipelineLayout(m_ctx.device, &layoutInfo, nullptr, &m_pipelineLayout);

    ENGINE_LOG_INFO("VulkanRHI::Initialization success");
}

void VulkanRHI::shutdown() {
    vkDeviceWaitIdle(m_ctx.device);
    m_uniformBuffer.clear(); // Destroy uniform buffers before device
    for (auto& sync : m_syncObjects) {
        vkDestroySemaphore(m_ctx.device, sync.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_ctx.device, sync.renderFinishedSemaphore, nullptr);
        vkDestroyFence(m_ctx.device, sync.inFlightFence, nullptr);
    }
    vkDestroyPipelineLayout(m_ctx.device, m_pipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_ctx.device, m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_ctx.device, m_descriptorSetLayout, nullptr);
    vkDestroyCommandPool(m_ctx.device, m_cmdPool, nullptr);
    for (auto view : m_ctx.swapchainImageViews) {
        vkDestroyImageView(m_ctx.device, view, nullptr);
    }
    vkDestroySwapchainKHR(m_ctx.device, m_ctx.swapchain, nullptr);
    vkDestroyDevice(m_ctx.device, nullptr);
    vkDestroySurfaceKHR(m_ctx.instance, m_ctx.surface, nullptr);
    vkb::destroy_debug_utils_messenger(m_ctx.instance, m_ctx.debugMessenger);
    vkDestroyInstance(m_ctx.instance, nullptr);
}

void VulkanRHI::beginFrame() {
    ENGINE_LOG_DEBUG("VulkanRHI::Begin of frame");

    auto& sync = m_syncObjects[m_currentFrame];
    // Wait fot gpu
    vkWaitForFences(m_ctx.device, 1, &sync.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_ctx.device, 1, &sync.inFlightFence);

    // Get image form swapchain
    vkAcquireNextImageKHR(m_ctx.device, m_ctx.swapchain, UINT64_MAX, sync.imageAvailableSemaphore, VK_NULL_HANDLE, &m_imageIndex);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_cmdBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to begin recording command buffer!");
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    transitionImageLayout(
        m_cmdBuffers[m_currentFrame],
        m_ctx.swapchainImages[m_imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT
    );

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_ctx.swapchainImageViews[m_imageIndex];
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = { { 0.1f, 0.1f, 0.12f, 1.0f } };

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = { {0, 0}, {WIDTH, HEIGHT} };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    vkCmdBeginRendering(m_cmdBuffers[m_currentFrame], &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(WIDTH);
    viewport.height = static_cast<float>(HEIGHT);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {WIDTH ,HEIGHT};

    vkCmdSetViewport(m_cmdBuffers[m_currentFrame], 0, 1, &viewport);
    vkCmdSetScissor(m_cmdBuffers[m_currentFrame], 0, 1, &scissor);
}

void VulkanRHI::endFrame() {
    vkCmdEndRendering(m_cmdBuffers[m_currentFrame]);

    transitionImageLayout(
        m_cmdBuffers[m_currentFrame],
        m_ctx.swapchainImages[m_imageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
        0
    );
    vkEndCommandBuffer(m_cmdBuffers[m_currentFrame]);

    auto& sync = m_syncObjects[m_currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Wait for image
    VkSemaphore waitSemaphores[] = { sync.imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_cmdBuffers[m_currentFrame];

    VkSemaphore signalSemaphores[] = { sync.renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(m_ctx.graphicsQueue, 1, &submitInfo, sync.inFlightFence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_ctx.swapchain;
    presentInfo.pImageIndices = &m_imageIndex;

    vkQueuePresentKHR(m_ctx.presentQueue, &presentInfo);

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    glfwPollEvents();
    ENGINE_LOG_DEBUG("VulkanRHI::End of frame");
}

void VulkanRHI::clear() {

}


/******** Draw ********/
void VulkanRHI::draw(std::shared_ptr<Pipeline> pipeline) {
    VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline.get());
    //vkCmdDrawIndexed(m_cmdBuffer, vulkanPipeline->getBindedNumberOfVerticles(), 1, 0, 0, 0);
    vkCmdDraw(m_cmdBuffers[m_currentFrame], vulkanPipeline->getBindedNumberOfVerticles(), 1, 0, 0);
}

/******** Buffer ********/
std::shared_ptr<Buffer> VulkanRHI::createBuffer(float vertices[], size_t size) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating buffer...");
    return std::make_shared<VulkanBuffer>(size, vertices, m_ctx, VulkanBufferType::VERTEX);
}

void VulkanRHI::bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) {
    ENGINE_LOG_TRACE("VulkanRHI::Binding vertex buffer...");
    VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline.get());

    vulkanPipeline->setCmdBuffer(m_cmdBuffers[m_currentFrame]);
    vulkanPipeline->bindVertexBuffer(buffer);
}

/******** PIPELINE/SHADER ********/
std::shared_ptr<Pipeline> VulkanRHI::createPipeline(PipelineInfo &info)
{
    ENGINE_LOG_TRACE("VulkanRHI::Creating pipeline...");
    return std::make_shared<VulkanPipeline>(info, m_ctx, m_pipelineLayout);
}

void VulkanRHI::bindPipeline(Pipeline *pipeline) {
    ENGINE_LOG_TRACE("VulkanRHI::Binding pipeline...");
    VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
    m_currentPipeline = vulkanPipeline;
    vkCmdBindPipeline(m_cmdBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getPipeline());
}

std::shared_ptr<Shader> VulkanRHI::createShader(ShaderType type, std::string source) {
    return std::make_shared<VulkanShader>(m_ctx.device, type, source);
}

/******** Unfiform ********/
void VulkanRHI::setGlobalUniform(const void* data, size_t size) {
    m_uniformBuffer[m_currentFrame]->setData(size, data, 0);
    vkCmdBindDescriptorSets(m_cmdBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[m_currentFrame], 0, nullptr);
}

void VulkanRHI::setLocalUniform(const void* data, size_t size) {
    vkCmdPushConstants(m_cmdBuffers[m_currentFrame], m_currentPipeline->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, (uint32_t)size, data);
}




/******** Utility ********/
void VulkanRHI::transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess) {
    VkImageMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.srcStageMask = srcStage;
    barrier.srcAccessMask = srcAccess;
    barrier.dstStageMask = dstStage;
    barrier.dstAccessMask = dstAccess;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &barrier;

    vkCmdPipelineBarrier2(cmd, &depInfo);
}