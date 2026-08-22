#pragma once
#include "core/rhi/rhi_buffer.h"
 
#include <vulkan/vulkan.h>
#include <iostream>

namespace vulkan {
    struct VulkanContext;
}

enum class VulkanBufferType {
    VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    UNIFORM = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
};

class VulkanBuffer : public Buffer {
    public:
    VulkanBuffer() = delete;
    VulkanBuffer(size_t size, const void* data, const vulkan::VulkanContext& ctx, VulkanBufferType type);
    ~VulkanBuffer();

    void setData(size_t size, const void* data, size_t offset = 0) override;
    size_t getSize() override { return m_size; };

    const VkBuffer* getBufferHandle() const { return &m_buffer; } 

private :
    const vulkan::VulkanContext& m_ctx;
    const VulkanBufferType m_type;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
    size_t m_size = 0;
    void* m_bufferData = nullptr;
};