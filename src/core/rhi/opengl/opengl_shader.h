#pragma once
#include "core/rhi/rhi_shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <unordered_map>

inline short shaderTypeToGLType(ShaderType type) {
    std::unordered_map<ShaderType, short> map = {
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