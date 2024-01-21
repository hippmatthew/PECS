/*
 *  PECS - graphicspipeline.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#include "src/include/pipeline.hpp"

namespace pecs
{

GraphicsPipeline::GraphicsPipeline(const vk::raii::Device& vk_device,
                                   const vk::Extent2D& vk_extent,
                                   const vk::Format& vk_format,
                                   const ShaderPaths& s) : Pipeline(s)
{
  auto vk_shaderModules = createShaderModules(vk_device);

  if (vk_shaderModules.empty())
    throw std::runtime_error("pecs : error @ GraphicsPipeline::GraphicsPipeline : shader modules failed to be created");

  auto ci_shaderStages = getShaderStageCreateInfos(vk_shaderModules);

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

  vk::PipelineVertexInputStateCreateInfo ci_vertexInput{
    .vertexBindingDescriptionCount    = 0,
    .pVertexBindingDescriptions       = nullptr,
    .vertexAttributeDescriptionCount  = 0,
    .pVertexAttributeDescriptions     = nullptr
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

  vk::PipelineLayoutCreateInfo ci_layout{
    .setLayoutCount         = 0,
    .pSetLayouts            = nullptr,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges    = nullptr
  };

  vk_layout = vk_device.createPipelineLayout(ci_layout);

  vk::PipelineRenderingCreateInfo ci_rendering{
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &vk_format
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
    .layout               = *vk_layout,
    .renderPass           = nullptr
  };

  vk_pipeline = vk_device.createGraphicsPipeline(nullptr, ci_pipeline);
};

} // namespace pecs