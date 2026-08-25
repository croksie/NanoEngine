#pragma once
#include "rhi/rhi_texture.h"
#include "rhi/vulkan/vulkan_initializer.h"
#include "rhi/vulkan/vulkan_buffer.h"

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace vulkan {
    struct VulkanContext;
}

inline VkFormat bufferTypeToVulkanType(TextureFormat format) {
    std::unordered_map<TextureFormat, VkFormat> map = {
        {TextureFormat::RGBA8_UNORM, VK_FORMAT_R8G8B8A8_UNORM},
        {TextureFormat::RGBA8_SRGB, VK_FORMAT_R8G8B8A8_SRGB},
        {TextureFormat::Depth24_Stencil8, VK_FORMAT_D24_UNORM_S8_UINT},
        {TextureFormat::Depth32F, VK_FORMAT_D32_SFLOAT}
    };
    return map[format];
};

class VulkanTexture : public Texture {
public:
    VulkanTexture(const TextureDesc& desc, vulkan::VulkanContext& ctx);
    ~VulkanTexture();

    VkImageView getImageView() const { return m_imageView; }
    VkSampler getSampler() const { return m_textureSampler; }

private:
    vulkan::VulkanContext* m_ctx;
    TextureDesc m_desc;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_textureSampler = VK_NULL_HANDLE;
};