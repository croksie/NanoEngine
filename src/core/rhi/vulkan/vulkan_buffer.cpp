#include "core/rhi/vulkan/vulkan_buffer.h"

#include "utils/log.h"
#include "vulkan_buffer.h"

// TODO : Handle memory allcoation error
VulkanBuffer::VulkanBuffer(size_t size, const void* data, const VulkanContext& ctx) : m_ctx(ctx), m_size(size) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(m_ctx.device, &bufferInfo, nullptr, &m_buffer);
    if(result != VK_SUCCESS) {
        ENGINE_LOG_ERROR("VulkanRHI::Failed to create buffer. Vulkan error Code : {}", static_cast<int>(result));
        throw std::runtime_error("Failed to create buffer");
    }

    // Get Memory Requierement and Allocate Memory
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(ctx.device, m_buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ctx);

    result = vkAllocateMemory(ctx.device, &allocInfo, nullptr, &m_bufferMemory) ;
    if (result != VK_SUCCESS) {
        ENGINE_LOG_ERROR("VulkanRHI::Failed to allocate buffer memory? Vulkan error Code : {}", static_cast<int>(result));
        throw std::runtime_error("Failed to allocate buffer memory");
    }

    result = vkBindBufferMemory(ctx.device, m_buffer, m_bufferMemory, 0);
    if (result != VK_SUCCESS) {
        ENGINE_LOG_ERROR("VulkanRHI::Failed to bind buffer memory? Vulkan error Code : {}", static_cast<int>(result));
        throw std::runtime_error("Failed to bind buffer memory");
    }

    // Copy data to buffer
    void* buffer_data;
    result = vkMapMemory(m_ctx.device, m_bufferMemory, 0, size, 0, &buffer_data) ;
    if (result != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanBuffer::Failed to map memory. Vulkan error Code : {}", static_cast<int>(result));
        throw std::runtime_error("Failed to map memory");
    }
    memcpy(buffer_data, data, (size_t) bufferInfo.size);
    vkUnmapMemory(ctx.device, m_bufferMemory);

}

VulkanBuffer::~VulkanBuffer() {
    vkDeviceWaitIdle(m_ctx.device);
    vkDestroyBuffer(m_ctx.device, m_buffer, nullptr);
    vkFreeMemory(m_ctx.device, m_bufferMemory, nullptr);
}

void VulkanBuffer::setData(size_t size, const void *data, size_t offset) {
    if (offset + size > m_size) {
        ENGINE_LOG_ERROR("VulkanRHI::Buffer overflow, data was not set.");
        return;
    }
    void* buffer_data;
    VkResult result = vkMapMemory(m_ctx.device, m_bufferMemory, offset, size, 0, &buffer_data); 
    if(result != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanBuffer::Failed to map memory. Vulkan error Code : {}", static_cast<int>(result));
        throw std::runtime_error("Failed to map memory");
    }
    memcpy(buffer_data, data, (size_t) size);
    vkUnmapMemory(m_ctx.device, m_bufferMemory);
}

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VulkanContext& ctx) {
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
