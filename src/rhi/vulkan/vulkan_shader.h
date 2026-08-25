#pragma once
#include "rhi/rhi_shader.h"

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include <string>
#include <unordered_map>

inline VkShaderStageFlagBits shaderTypeToVKType(ShaderType type) {
    std::unordered_map<ShaderType, VkShaderStageFlagBits> map = {
        {ShaderType::VERTEX, VK_SHADER_STAGE_VERTEX_BIT},
        {ShaderType::FRAGMENT, VK_SHADER_STAGE_FRAGMENT_BIT}
    };
    return map[type];
}

class VulkanShader : public Shader {
public:
    VulkanShader(VkDevice device, ShaderType type, const std::vector<uint32_t>& spirvCode, const std::string& entryPoint = "main");
    
    // Tempory constructor that will be remplaced by dedicated system
    VulkanShader(VkDevice device, ShaderType type, const std::string& source, const std::string& entryPoint = "main");
    ~VulkanShader();

    VkPipelineShaderStageCreateInfo getStageCreateInfo() const;

    VkShaderModule getModule() { return m_module; }


    
private:
    // Tempory function that will be remplaced by dedicated system
    std::vector<uint32_t> compileGLSLToSPIRV(
        const std::string& source,
        shaderc_shader_kind kind,
        const std::string& filename = "shader.glsl",
        const std::string& entryPoint = "main");


    VkDevice m_device;
    ShaderType m_type;
    VkShaderModule m_module;
    std::string m_entryPoint;
};