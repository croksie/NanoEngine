#include "core/rhi/opengl/opengl_texture.h"

#include "utils/log.h"

OpenGLTexture::OpenGLTexture(const TextureDesc& desc) {
    ENGINE_LOG_INFO("OpenGLRHI::Creating texture...");
    m_desc = desc;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
    glTextureStorage2D(m_textureID, 1, GL_RGBA8, m_desc.width, m_desc.height);
    glTextureSubImage2D(m_textureID, 0, 0, 0, m_desc.width, m_desc.height, GL_RGBA, GL_UNSIGNED_BYTE, m_desc.initialData);

    glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTextureUnit(1, m_textureID);

    ENGINE_LOG_INFO("OpenGLRHI::Texture created");
}

OpenGLTexture::~OpenGLTexture() {
    glDeleteTextures(1, &m_textureID);
}

