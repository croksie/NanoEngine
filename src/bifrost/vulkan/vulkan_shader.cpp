#include "bifrost/vulkan/vulkan_shader.h"

#include <stdexcept>

#include "utils/log.h"

namespace midgard::bifrost::vulkan {

VulkanShader::VulkanShader(VkDevice device, ShaderType type, const std::vector<uint32_t>& spirvCode, const std::string &entryPoint)
    : m_device(device), m_type(type), m_entryPoint(entryPoint)
{
    ENGINE_LOG_TRACE("Creating shader...");
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
    createInfo.pCode = spirvCode.data();

    vkCreateShaderModule(m_device, &createInfo, nullptr, &m_module);
}

VulkanShader::VulkanShader(VkDevice device, ShaderType type, const std::string &source, const std::string &entryPoint)
    : m_device(device), m_type(type), m_entryPoint(entryPoint)
{
    std::vector<uint32_t> spirvCode;
    if (type == ShaderType::VERTEX) {
        spirvCode = compileGLSLToSPIRV(source, shaderc_vertex_shader, "shader.vert", entryPoint);
    } else if (type == ShaderType::FRAGMENT) {
        spirvCode = compileGLSLToSPIRV(source, shaderc_fragment_shader, "shader.frag", entryPoint);
    }

    ENGINE_LOG_TRACE("Creating shader...");
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
    createInfo.pCode = spirvCode.data();

    VkResult result = vkCreateShaderModule(m_device, &createInfo, nullptr, &m_module);
    if (result != VK_SUCCESS) {
        ENGINE_LOG_CRITICAL("Failed to create shader module. Vulkan error Code : {}", static_cast<int>(result));
    }
}

VulkanShader::~VulkanShader() {
    if (m_module != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, m_module, nullptr);
    }
}

VkPipelineShaderStageCreateInfo VulkanShader::getStageCreateInfo() const {
    VkPipelineShaderStageCreateInfo stageInfo{};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = shaderTypeToVKType(m_type);
    stageInfo.module = m_module;
    stageInfo.pName = m_entryPoint.c_str();
    return stageInfo;
}

std::vector<uint32_t> VulkanShader::compileGLSLToSPIRV(
    const std::string& source,
    shaderc_shader_kind kind,
    const std::string& filename,
    const std::string& entryPoint)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        source.c_str(),
        source.size(),
        kind,
        filename.c_str(),
        entryPoint.c_str(),
        options
    );

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        ENGINE_LOG_ERROR("Failed to compile shader : {}", result.GetErrorMessage());
        return {};
    }

    return {result.cbegin(), result.cend()};
}

} // namespace midgard::bifrost::vulkan