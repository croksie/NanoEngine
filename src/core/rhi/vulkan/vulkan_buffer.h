#pragma once
#include "core/rhi/rhi_buffer.h"

#include <core/rhi/vulkan/vulkan_rhi.h>

#include <vulkan/vulkan.h>

#include <iostream>

struct VulkanContext;

class VulkanBuffer : public Buffer {
    public:
    VulkanBuffer() = delete;
    VulkanBuffer(size_t size, const void* data, const VulkanContext& ctx);
    ~VulkanBuffer() ;

    void setData(size_t size, const void* data, size_t offset = 0) override;
    size_t getSize() override { return m_size; };

    const VkBuffer* getBufferHandle() const { return &m_buffer; } 

private :
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VulkanContext& ctx);

    const VulkanContext& m_ctx;

    VkBuffer m_buffer;
    VkDeviceMemory m_bufferMemory;
    size_t m_size{0};
};