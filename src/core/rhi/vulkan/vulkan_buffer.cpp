#include "core/rhi/vulkan/vulkan_buffer.h"
#include "core/rhi/vulkan/vulkan_initializer.h"

#include "utils/log.h"

// TODO: Handle memory allcoation error
VulkanBuffer::VulkanBuffer(size_t size, const void* data, const vulkan::VulkanContext& ctx, VulkanBufferType type) : m_ctx(ctx), m_size(size), m_type(type) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = static_cast<uint32_t>(type); // Get from VulkanBufferType Enum
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
    allocInfo.memoryTypeIndex = vulkan::findMemoryType(ctx, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

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

    result = vkMapMemory(m_ctx.device, m_bufferMemory, 0, size, 0, &m_bufferData);
    if (result != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("VulkanBuffer::Failed to map memory. Vulkan error Code : {}", static_cast<int>(result));
        throw std::runtime_error("Failed to map memory");
    }
    
    // Copy data to buffer if data is provided
    if (data != nullptr) {
        memcpy(m_bufferData, data, (size_t) bufferInfo.size);
        //vkUnmapMemory(ctx.device, m_bufferMemory);
    }
}

VulkanBuffer::~VulkanBuffer() {
    if (m_buffer != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_ctx.device);
        vkUnmapMemory(m_ctx.device, m_bufferMemory);
        vkDestroyBuffer(m_ctx.device, m_buffer, nullptr);
        vkFreeMemory(m_ctx.device, m_bufferMemory, nullptr);
    }
}

void VulkanBuffer::setData(size_t size, const void *data, size_t offset) {
    if (offset + size > m_size) {
        ENGINE_LOG_ERROR("VulkanRHI::Buffer overflow, data was not set.");
        return;
    }
    //void* buffer_data;
    // VkResult result = vkMapMemory(m_ctx.device, m_bufferMemory, offset, size, 0, &buffer_data); 
    // if(result != VK_SUCCESS) {
    //     ENGINE_LOG_CRITICAL("VulkanBuffer::Failed to map memory. Vulkan error Code : {}", static_cast<int>(result));
    //     throw std::runtime_error("Failed to map memory");
    // }
    memcpy(m_bufferData, data, (size_t) size);
    //vkUnmapMemory(m_ctx.device, m_bufferMemory);
}

