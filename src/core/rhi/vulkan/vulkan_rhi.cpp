#include "core/rhi/vulkan/vulkan_rhi.h"
#include "utils/log.h"

using namespace vulkan;

void VulkanRHI::initialize(Window *window) {
    m_window = window;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_ctx = vulkan::createContext(window); // Window created here

    m_window->setWindowSizeCallback([this](int width, int height) {
        this->onWindowResize(width, height);
    });

    int width, height;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_window->getNativeHandle()), &width, &height);
    m_width = width;
    m_height = height;

    vulkan::createSwapchain(m_ctx, m_width, m_height);
    vulkan::createDepthBuffer(m_ctx, m_width, m_height);
    vulkan::createFenceAndSemaphore(m_ctx);
    vulkan::createCommandPool(m_ctx);
    vulkan::createCommandBuffer(m_ctx);
    vulkan::createUniformBuffers(m_ctx);
    vulkan::createDescriptorSetLayout(m_ctx);
    vulkan::createDescriptorPool(m_ctx);
    vulkan::createDescriptorSets(m_ctx);
    vulkan::createPipelineLayout(m_ctx);

    ENGINE_LOG_INFO("VulkanRHI::Initialization success");
}

void VulkanRHI::onWindowResize(int width, int height) {
    m_framebufferResized = true;
    m_height = height;
    m_width = width;
}

void VulkanRHI::shutdown() {
    vkDeviceWaitIdle(m_ctx.device);
    m_ctx.uniformBuffers.clear(); // Destroy uniform buffers before device
    for (auto& sync : m_ctx.syncObjects) {
        vkDestroySemaphore(m_ctx.device, sync.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_ctx.device, sync.renderFinishedSemaphore, nullptr);
        vkDestroyFence(m_ctx.device, sync.inFlightFence, nullptr);
    }
    vkDestroyPipelineLayout(m_ctx.device, m_ctx.pipelineLayout, nullptr);
    vkDestroyDescriptorPool(m_ctx.device, m_ctx.descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_ctx.device, m_ctx.descriptorSetLayout, nullptr);
    vkDestroyCommandPool(m_ctx.device, m_ctx.cmdPool, nullptr);
    for (auto view : m_ctx.swapchainImageViews) {
        vkDestroyImageView(m_ctx.device, view, nullptr);
    }
    vkDestroySwapchainKHR(m_ctx.device, m_ctx.swapchain, nullptr);
    for (size_t i = 0; i < m_ctx.depthImageViews.size(); i++) {
        if (m_ctx.depthImageViews[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(m_ctx.device, m_ctx.depthImageViews[i], nullptr);
        }
        if (m_ctx.depthImages[i] != VK_NULL_HANDLE) {
            vkDestroyImage(m_ctx.device, m_ctx.depthImages[i], nullptr);
        }
        if (m_ctx.depthImageMemories[i] != VK_NULL_HANDLE) {
            vkFreeMemory(m_ctx.device, m_ctx.depthImageMemories[i], nullptr);
        }
    }
    m_ctx.depthImageViews.clear();
    m_ctx.depthImages.clear();
    m_ctx.depthImageMemories.clear();
    vkDestroyDevice(m_ctx.device, nullptr);
    vkDestroySurfaceKHR(m_ctx.instance, m_ctx.surface, nullptr);
    vkb::destroy_debug_utils_messenger(m_ctx.instance, m_ctx.debugMessenger);
    vkDestroyInstance(m_ctx.instance, nullptr);
}

/***************************************************************
 ********************** RENDER FUNCTION ***********************
 ***************************************************************/
void VulkanRHI::beginFrame() {
    ENGINE_LOG_TRACE("VulkanRHI::Begin of frame");

    if (m_framebufferResized) {
        ENGINE_LOG_DEBUG("VulkanRHI::Windows resized");

        vulkan::cleanupSwapchain(m_ctx);
        vulkan::cleanupDepthBuffer(m_ctx);

        vulkan::createSwapchain(m_ctx, m_width, m_height);
        vulkan::createDepthBuffer(m_ctx, m_width, m_height);
        m_framebufferResized = false;
    }

    auto& sync = m_ctx.syncObjects[m_currentFrame];
    // Wait for gpu
    vkWaitForFences(m_ctx.device, 1, &sync.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_ctx.device, 1, &sync.inFlightFence);

    // Get image from swapchain
    vkAcquireNextImageKHR(m_ctx.device, m_ctx.swapchain, UINT64_MAX, sync.imageAvailableSemaphore, VK_NULL_HANDLE, &m_imageIndex);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(m_ctx.cmdBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanRHI::Failed to begin recording command buffer!");
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    transitionImageLayout(
        m_ctx.cmdBuffers[m_currentFrame],
        m_ctx.swapchainImages[m_imageIndex],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        0,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    transitionImageLayout(
        m_ctx.cmdBuffers[m_currentFrame],
        m_ctx.depthImages[m_currentFrame],
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        0,
        VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_ctx.swapchainImageViews[m_imageIndex];
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = { { 0.1f, 0.1f, 0.12f, 1.0f } };

    VkRenderingAttachmentInfo depthAttachment{};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.imageView = m_ctx.depthImageViews[m_currentFrame];
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.clearValue.depthStencil = { 1.0f, 0 };

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = { {0, 0}, {m_width, m_height} };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = &depthAttachment;

    vkCmdBeginRendering(m_ctx.cmdBuffers[m_currentFrame], &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(m_width);
    viewport.height = static_cast<float>(m_height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {m_width ,m_height};

    vkCmdSetViewport(m_ctx.cmdBuffers[m_currentFrame], 0, 1, &viewport);
    vkCmdSetScissor(m_ctx.cmdBuffers[m_currentFrame], 0, 1, &scissor);
}

void VulkanRHI::endFrame() {
    vkCmdEndRendering(m_ctx.cmdBuffers[m_currentFrame]);

    transitionImageLayout(
        m_ctx.cmdBuffers[m_currentFrame],
        m_ctx.swapchainImages[m_imageIndex],
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
        0
    );
    vkEndCommandBuffer(m_ctx.cmdBuffers[m_currentFrame]);

    auto& sync = m_ctx.syncObjects[m_currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Wait for image
    VkSemaphore waitSemaphores[] = { sync.imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_ctx.cmdBuffers[m_currentFrame];

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
    ENGINE_LOG_TRACE("VulkanRHI::End of frame");
}

void VulkanRHI::clear() {

}

/******** Draw ********/
void VulkanRHI::draw(std::shared_ptr<Pipeline> pipeline) {
    VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline.get());
    vkCmdDraw(m_ctx.cmdBuffers[m_currentFrame], vulkanPipeline->getBindedNumberOfVerticles(), 1, 0, 0);
}

/******** Buffer ********/
std::shared_ptr<Buffer> VulkanRHI::createBuffer(float vertices[], size_t size) {
    ENGINE_LOG_TRACE("VulkanRHI::Creating buffer...");
    return std::make_shared<VulkanBuffer>(size, vertices, m_ctx, VulkanBufferType::VERTEX);
}

void VulkanRHI::bindVertexBuffer(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<Buffer> buffer) {
    ENGINE_LOG_TRACE("VulkanRHI::Binding vertex buffer...");
    VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline.get());

    vulkanPipeline->setCmdBuffer(m_ctx.cmdBuffers[m_currentFrame]);
    vulkanPipeline->bindVertexBuffer(buffer);
}

/******** PIPELINE/SHADER ********/
std::shared_ptr<Pipeline> VulkanRHI::createPipeline(PipelineInfo &info)
{
    ENGINE_LOG_TRACE("VulkanRHI::Creating pipeline...");
    return std::make_shared<VulkanPipeline>(info, m_ctx, m_ctx.pipelineLayout);
}

void VulkanRHI::bindPipeline(Pipeline *pipeline) {
    ENGINE_LOG_TRACE("VulkanRHI::Binding pipeline...");
    VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
    m_currentPipeline = vulkanPipeline;
    vkCmdBindPipeline(m_ctx.cmdBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->getPipeline());
}

std::shared_ptr<Shader> VulkanRHI::createShader(ShaderType type, std::string source) {
    return std::make_shared<VulkanShader>(m_ctx.device, type, source);
}

/******** Uniform ********/
void VulkanRHI::setGlobalUniform(const void* data, size_t size) {
    m_ctx.uniformBuffers[m_currentFrame]->setData(size, data, 0);
    vkCmdBindDescriptorSets(m_ctx.cmdBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_ctx.pipelineLayout, 0, 1, &m_ctx.descriptorSets[m_currentFrame], 0, nullptr);
}

void VulkanRHI::setLocalUniform(const void* data, size_t size) {
    vkCmdPushConstants(m_ctx.cmdBuffers[m_currentFrame], m_currentPipeline->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, (uint32_t)size, data);
}

/******** Utility ********/
uint32_t VulkanRHI::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_ctx.physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    ENGINE_LOG_CRITICAL("VulkanRHI::Failed to find suitable memory type");
    throw std::runtime_error("Failed to find suitable memory type");
}

void VulkanRHI::transitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess, VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess, VkImageAspectFlags aspectMask) {
    VkImageMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.srcStageMask = srcStage;
    barrier.srcAccessMask = srcAccess;
    barrier.dstStageMask = dstStage;
    barrier.dstAccessMask = dstAccess;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspectMask;
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
