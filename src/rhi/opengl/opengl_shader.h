#pragma once
#include "rhi/rhi_shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <unordered_map>

inline uint32_t shaderTypeToGLType(ShaderType type) {
    std::unordered_map<ShaderType, uint32_t> map = {
        {ShaderType::VERTEX, GL_VERTEX_SHADER},
        {ShaderType::FRAGMENT, GL_FRAGMENT_SHADER}
    };
    return map[type];
}

class OpenGLShader : public Shader {
public:
    OpenGLShader(ShaderType type, const std::string& source);
    ~OpenGLShader();

    GLuint getShaderID() { return m_shaderID; }


private:
    GLuint m_shaderID;
};