#include "bifrost/opengl/opengl_shader.h"

#include "utils/log.h"

namespace midgard::bifrost::opengl {

OpenGLShader::OpenGLShader(ShaderType type, const std::string &source) {
    ENGINE_LOG_TRACE("Creating shader...");
    m_shaderID = glCreateShader(shaderTypeToGLType(type));
    const GLchar* shaderSource = source.c_str();
    glShaderSource(m_shaderID, 1, &shaderSource, NULL);
    glCompileShader(m_shaderID);

    ENGINE_LOG_TRACE("Shader created");
}

OpenGLShader::~OpenGLShader() {
    if (m_shaderID) {
        glDeleteShader(m_shaderID);
    }
}

} // namespace midgard::bifrost::opengl

