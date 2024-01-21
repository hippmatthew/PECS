/*
 *  PECS - pipeline.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#include "src/include/pipeline.hpp"

namespace pecs
{

std::vector<char> Pipeline::readShader(std::string path) const
{ 
  std::vector<char> buffer;

  std::ifstream shader(path, std::ios::ate | std::ios::binary);
  if (shader.fail()) return buffer;

  std::size_t size = shader.tellg();
  buffer.resize(size);

  shader.seekg(0);
  shader.read(buffer.data(), size);

  shader.close();
  return buffer;
}

std::vector<vk::raii::ShaderModule> Pipeline::createShaderModules(const vk::raii::Device& vk_device) const
{
  // currently does not support compute shaders
  
  std::vector<vk::raii::ShaderModule> modules;
  
  if (!shaderPaths.vertex.has_value()) return modules;
  
  auto vertexShader = readShader(shaderPaths.vertex.value());
  vk::ShaderModuleCreateInfo ci_vertexModule{
    .codeSize = vertexShader.size(),
    .pCode    = reinterpret_cast<const unsigned int *>(vertexShader.data())
  };

  std::vector<char> fragmentShader;
  if (shaderPaths.fragment.has_value())
    fragmentShader = readShader(shaderPaths.fragment.value());
  else
    fragmentShader = readShader("./shaders/fragment/missing.frag");

  vk::ShaderModuleCreateInfo ci_fragmentModule{
    .codeSize = fragmentShader.size(),
    .pCode    = reinterpret_cast<unsigned int *>(fragmentShader.data())
  };

  modules.emplace_back(vk_device.createShaderModule(ci_vertexModule));
  modules.emplace_back(vk_device.createShaderModule(ci_fragmentModule));

  return modules;
}

std::vector<vk::PipelineShaderStageCreateInfo> Pipeline::getShaderStageCreateInfos(const std::vector<vk::raii::ShaderModule>& vk_shaderModules) const
{
  std::vector<vk::PipelineShaderStageCreateInfo> createInfos;

  if (vk_shaderModules.size() != 2) return createInfos; // temporary check until compute shader support is added

  vk::PipelineShaderStageCreateInfo ci_vertexStage{
    .stage  = vk::ShaderStageFlagBits::eVertex,
    .module = *vk_shaderModules[0],
    .pName  = "main"
  };
  createInfos.emplace_back(ci_vertexStage);

  vk::PipelineShaderStageCreateInfo ci_fragmentStage{
    .stage  = vk::ShaderStageFlagBits::eFragment,
    .module = *vk_shaderModules[1],
    .pName  = "main"
  };
  createInfos.emplace_back(ci_fragmentStage);

  return createInfos;
}

} // namespace pecs