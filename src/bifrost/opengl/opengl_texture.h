#pragma once
#include <glad/glad.h>

#include "bifrost/texture.h"

namespace midgard::bifrost::opengl {

class OpenGLTexture : public Texture {
public:
    OpenGLTexture(const TextureDesc& desc);
    ~OpenGLTexture() override;

    GLuint getTextureID() const { return m_textureID; }

private:
    GLuint m_textureID = 0;
    TextureDesc m_desc;
};

} // namespace midgard::bifrost::opengl