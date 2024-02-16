/*
 *  PECS::core - object.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/26/24
 *  Updated:  2/12/24
 */

#include "src/core/include/object.hpp"

namespace pecs
{

Object::Object(ShaderPaths s) : shaderPaths(s) {}

std::vector<char> Object::readShader(std::string path) const
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

Object::Object(const Object& o)
{
  shaderPaths = {
    o.shaderPaths.vertex,
    o.shaderPaths.fragment,
    o.shaderPaths.compute
  };

  position = o.position;
  
  vertices = o.vertices;
  indices = o.indices;
}

Object::Object(Object&& o)
{
  shaderPaths = {
    o.shaderPaths.vertex,
    o.shaderPaths.fragment,
    o.shaderPaths.compute
  };

  o.shaderPaths.vertex.reset();
  o.shaderPaths.fragment.reset();
  o.shaderPaths.vertex.reset();

  position = o.position;
  o.position = { 0.0f, 0.0f, 0.0f };

  nextTransformation = o.nextTransformation;
  o.nextTransformation = glm::mat4(1.0f);

  hasTransformed = o.hasTransformed;
  o.hasTransformed = false;

  vertices = o.vertices;
  indices = o.indices;
  
  objectData = {
    o.objectData.model,
    o.objectData.view,
    o.objectData.projection
  };

  o.objectData = {
    glm::mat4(1.0f),
    glm::mat4(1.0f),
    glm::mat4(1.0f)
  };
}

Object& Object::operator = (const Object& o)
{
  if (this == &o) return *this;
  
  vk_descriptorLayout.clear();
  vk_graphicsLayout.clear();
  vk_graphicsPipeline.clear();

  objectData = {
    glm::mat4(1.0f),
    glm::mat4(1.0f),
    glm::mat4(1.0f)
  };

  nextTransformation = glm::mat4(1.0f);
  hasTransformed = false;
  
  shaderPaths = {
    o.shaderPaths.vertex,
    o.shaderPaths.fragment,
    o.shaderPaths.compute
  };

  position = o.position;
  
  vertices = o.vertices;
  indices = o.indices;

  return *this;
}

Object& Object::operator = (Object&& o)
{
  if (this == &o) return *this;

  vk_descriptorLayout.clear();
  vk_graphicsLayout.clear();
  vk_graphicsPipeline.clear();
  
  shaderPaths = {
    o.shaderPaths.vertex,
    o.shaderPaths.fragment,
    o.shaderPaths.compute
  };
  
  o.shaderPaths.vertex.reset();
  o.shaderPaths.fragment.reset();
  o.shaderPaths.vertex.reset();

  position = o.position;
  o.position = { 0.0f, 0.0f, 0.0f };

  nextTransformation = o.nextTransformation;
  o.nextTransformation = glm::mat4(1.0f);

  hasTransformed = o.hasTransformed;
  o.hasTransformed = false;

  vertices = o.vertices;
  indices = o.indices;
  
  objectData = {
    o.objectData.model,
    o.objectData.view,
    o.objectData.projection
  };

  o.objectData = {
    glm::mat4(1.0f),
    glm::mat4(1.0f),
    glm::mat4(1.0f)
  };

  return *this;
}

void Object::translate(glm::vec3 translation)
{
  nextTransformation = glm::translate(nextTransformation, translation);
  hasTransformed = true;
}

void Object::rotate(RotationInfo rotation)
{
  nextTransformation = glm::rotate(nextTransformation, rotation.angle, rotation.axis);
  hasTransformed = true;
}

void Object::clean()
{
  vk_descriptorLayout.clear();
  vk_graphicsPipeline.clear();
  vk_graphicsLayout.clear();
}

std::vector<vk::raii::ShaderModule> Object::createShaderModules(const vk::raii::Device& vk_device) const
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
    fragmentShader = readShader("./shaders/missing.frag.spv");

  vk::ShaderModuleCreateInfo ci_fragmentModule{
    .codeSize = fragmentShader.size(),
    .pCode    = reinterpret_cast<unsigned int *>(fragmentShader.data())
  };

  modules.emplace_back(vk_device.createShaderModule(ci_vertexModule));
  modules.emplace_back(vk_device.createShaderModule(ci_fragmentModule));

  return modules;
}

std::vector<vk::PipelineShaderStageCreateInfo> Object::shaderCreateInfos(const std::vector<vk::raii::ShaderModule>& vk_shaderModules) const
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

void Object::createGraphicsPipeline(const vk::raii::Device& vk_device, const ViewportInfo& i_viewport)
{
  auto vk_shaderModules = createShaderModules(vk_device);

  if (vk_shaderModules.empty())
    throw std::runtime_error("pecs : error @ GraphicsPipeline::GraphicsPipeline : shader modules failed to be created");

  auto ci_shaderStages = shaderCreateInfos(vk_shaderModules);

  std::vector<vk::DynamicState> dynamicStates = { 
    vk::DynamicState::eViewport,
    vk::DynamicState::eScissor
  };

  vk::PipelineDynamicStateCreateInfo ci_dynamicState{
    .dynamicStateCount  = static_cast<unsigned int>(dynamicStates.size()),
    .pDynamicStates     = dynamicStates.data()
  };

  vk::PipelineViewportStateCreateInfo ci_viewportState{
    .viewportCount  = 1,
    .scissorCount   = 1
  };

  auto vk_bindingDescription = Vertex::bindingDescription();
  auto vk_attributeDescriptions = Vertex::attributeDescriptions();
  vk::PipelineVertexInputStateCreateInfo ci_vertexInput{
    .vertexBindingDescriptionCount    = 1,
    .pVertexBindingDescriptions       = &vk_bindingDescription,
    .vertexAttributeDescriptionCount  = static_cast<unsigned int>(vk_attributeDescriptions.size()),
    .pVertexAttributeDescriptions     = vk_attributeDescriptions.data()
  };

  vk::PipelineInputAssemblyStateCreateInfo ci_inputAssembly{
    .topology               = vk::PrimitiveTopology::eTriangleList,
    .primitiveRestartEnable = vk::False
  };

  vk::PipelineRasterizationStateCreateInfo ci_rasterizer{
    .depthClampEnable         = vk::False,
    .rasterizerDiscardEnable  = vk::False,
    .polygonMode              = vk::PolygonMode::eFill,
    .cullMode                 = vk::CullModeFlagBits::eBack,
    .frontFace                = vk::FrontFace::eCounterClockwise,
    .depthBiasEnable          = vk::False,
    .depthBiasConstantFactor  = 0.0f,
    .depthBiasClamp           = 0.0f,
    .depthBiasSlopeFactor     = 0.0f,
    .lineWidth                = 1.0f
  };

  vk::PipelineMultisampleStateCreateInfo ci_multisampling{
    .rasterizationSamples   = vk::SampleCountFlagBits::e1,
    .sampleShadingEnable    = vk::False,
    .minSampleShading       = 1.0f,
    .pSampleMask            = nullptr,
    .alphaToCoverageEnable  = vk::False,
    .alphaToOneEnable       = vk::False
  };

  vk::PipelineColorBlendAttachmentState blendState{
    .blendEnable          = vk::True,
    .srcColorBlendFactor  = vk::BlendFactor::eSrcAlpha,
    .dstColorBlendFactor  = vk::BlendFactor::eOneMinusSrcAlpha,
    .colorBlendOp         = vk::BlendOp::eAdd,
    .srcAlphaBlendFactor  = vk::BlendFactor::eOne,
    .dstAlphaBlendFactor  = vk::BlendFactor::eZero,
    .alphaBlendOp         = vk::BlendOp::eAdd,
    .colorWriteMask       = vk::ColorComponentFlagBits::eR |
                            vk::ColorComponentFlagBits::eG |
                            vk::ColorComponentFlagBits::eB |
                            vk::ColorComponentFlagBits::eA
  };

  vk::PipelineColorBlendStateCreateInfo ci_blendState{
    .logicOpEnable    = vk::False,
    .logicOp          = vk::LogicOp::eCopy,
    .attachmentCount  = 1,
    .pAttachments     = &blendState
  };

  vk::DescriptorSetLayoutBinding descriptorLayoutBinding{
    .binding          = 0,
    .descriptorType   = vk::DescriptorType::eUniformBuffer,
    .descriptorCount  = 1,
    .stageFlags       = vk::ShaderStageFlagBits::eVertex
  };

  vk::DescriptorSetLayoutCreateInfo ci_descriptorLayout{
    .bindingCount = 1,
    .pBindings    = &descriptorLayoutBinding
  };
  vk_descriptorLayout = vk_device.createDescriptorSetLayout(ci_descriptorLayout);

  vk::PipelineLayoutCreateInfo ci_pipelineLayout{
    .setLayoutCount         = 1,
    .pSetLayouts            = &*vk_descriptorLayout,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges    = nullptr
  };

  vk_graphicsLayout = vk_device.createPipelineLayout(ci_pipelineLayout);

  vk::PipelineRenderingCreateInfoKHR ci_rendering{
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &(i_viewport.second)
  };

  vk::GraphicsPipelineCreateInfo ci_pipeline{
    .pNext                = &ci_rendering,
    .stageCount           = static_cast<unsigned int>(ci_shaderStages.size()),
    .pStages              = ci_shaderStages.data(),
    .pVertexInputState    = &ci_vertexInput,
    .pInputAssemblyState  = &ci_inputAssembly,
    .pViewportState       = &ci_viewportState,
    .pRasterizationState  = &ci_rasterizer,
    .pMultisampleState    = &ci_multisampling,
    .pDepthStencilState   = nullptr,
    .pColorBlendState     = &ci_blendState,
    .pDynamicState        = &ci_dynamicState,
    .layout               = *vk_graphicsLayout,
  };

  vk_graphicsPipeline = vk_device.createGraphicsPipeline(nullptr, ci_pipeline);
}

} // namespace pecs