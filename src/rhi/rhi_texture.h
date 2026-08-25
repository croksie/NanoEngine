#pragma once
#include <cstdint>

enum class TextureFormat {
    RGBA8_UNORM,
    RGBA8_SRGB,
    Depth24_Stencil8,
    Depth32F
};

struct TextureDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    TextureFormat format = TextureFormat::RGBA8_SRGB;
    const void* initialData = nullptr;
    uint32_t size;
};

class Texture {
    
};