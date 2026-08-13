#include "core/rhi/opengl/opengl_shader.h"
#include "opengl_shader.h"

OpenGLShader::OpenGLShader(ShaderType type, const std::string &source) {
    m_shaderID = glCreateShader(shaderTypeToGLType(type));
    const GLchar* shaderSource = source.c_str();
    glShaderSource(m_shaderID, 1, &shaderSource, NULL);
    glCompileShader(m_shaderID);
}

OpenGLShader::~OpenGLShader() {
    glDeleteShader(m_shaderID);
}
