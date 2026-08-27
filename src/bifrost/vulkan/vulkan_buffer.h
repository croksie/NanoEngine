#pragma once

#include <vulkan/vulkan.h>

#include "utils/log.h"

#include "bifrost/buffer.h"

namespace midgard::bifrost::vulkan {
    struct VulkanContext;

inline VkBufferUsageFlagBits bufferTypeToVulkanType(BufferType type) {
    switch (type) {
        case BufferType::VERTEX: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case BufferType::INDEX: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case BufferType::UNIFORM: return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case BufferType::STORAGE: return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        case BufferType::TRANSFER: return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        default: 
            ENGINE_LOG_ERROR("Invalid buffer type");
            return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }
}

class VulkanBuffer : public Buffer {
public:
    VulkanBuffer() = delete;
    VulkanBuffer(const BufferDesc& desc, const VulkanContext& ctx);
    ~VulkanBuffer() override;

    void setData(size_t size, const void* data, size_t offset = 0) override;
    size_t getSize() const override { return m_desc.size; }
    BufferType getType() const override { return m_desc.type; }

    VkBuffer* getBufferHandle() { return &m_buffer; } 
    VkBuffer getBuffer() const { return m_buffer; }

private:
    const VulkanContext& m_ctx;
    BufferDesc m_desc;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_bufferMemory = VK_NULL_HANDLE;
    void* m_bufferData = nullptr;
};

} // namespace midgard::bifrost::vulkan