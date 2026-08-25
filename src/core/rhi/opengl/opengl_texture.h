#pragma once
#include "core/rhi/rhi_texture.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"


class OpenGLTexture : public Texture {
public:
    OpenGLTexture(const TextureDesc& desc);
    ~OpenGLTexture();

    GLuint getTextureID() const { return m_textureID; }

private:
    GLuint m_textureID;
    TextureDesc m_desc;
};