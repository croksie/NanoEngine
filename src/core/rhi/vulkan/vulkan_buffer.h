#pragma once
#include "core/rhi/rhi_buffer.h"
 
#include <vulkan/vulkan.h>
#include <iostream>

#include <unordered_map>

namespace vulkan {
    struct VulkanContext;
}

inline VkBufferUsageFlagBits bufferTypeToVulkanType(BufferType type) {
    std::unordered_map<BufferType, VkBufferUsageFlagBits> map = {
        {BufferType::VERTEX, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT},
        {BufferType::INDEX, VK_BUFFER_USAGE_INDEX_BUFFER_BIT},
        {BufferType::UNIFORM, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT}
    };
    return map[type];
};

class VulkanBuffer : public Buffer {
    public:
    VulkanBuffer() = delete;
    VulkanBuffer(const BufferDesc& desc, const vulkan::VulkanContext& ctx);
    ~VulkanBuffer();

    void setData(size_t size, const void* data, size_t offset = 0) override;
    size_t getSize() const override { return m_desc.size; };
    BufferType getType() const override { return m_desc.type; };

    VkBuffer* getBufferHandle() { return &m_buffer; } 

private :
    const vulkan::VulkanContext& m_ctx;
    BufferDesc m_desc;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
    void* m_bufferData = nullptr;
};