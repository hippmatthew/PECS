#include "src/core/include/component.hpp"

namespace pecs
{

GraphicsComponent::GraphicsComponent(const GraphicsShaders& p, const std::vector<Vertex>& v, const std::vector<unsigned int>& i)
: paths(p), vertices(v), indices(i)
{}

GraphicsComponent& GraphicsComponent::translateModel(glm::vec3 t)
{
  model = glm::translate(model, t);
  
  return *this;
}

GraphicsComponent& GraphicsComponent::rotateModel(float angle, glm::vec3 axis)
{
  model = glm::rotate(model, angle, axis);

  return *this;
}

void GraphicsComponent::initialize(const Device& device, const ViewportInfo& vi, const vk::raii::Buffer& vk_cameraBuffer, unsigned long maxFlightFrames)
{
  i_viewport = vi;

  createPipeline(device.logical());
  allocateBuffers(device);
  createDescriptorSets(device.logical(), maxFlightFrames, vk_cameraBuffer);
}

ShaderModules GraphicsComponent::createShaderModules(const vk::raii::Device& vk_device) const
{
  ShaderModules modules;
    
  int shaderIndex = 0;
  for (auto path : paths.array())
  {
    vk::ShaderStageFlagBits stage;
    switch (shaderIndex++)
    {
      case 0:
        stage = vk::ShaderStageFlagBits::eVertex;
        break;
      case 1:
        stage = vk::ShaderStageFlagBits::eTessellationControl;
        break;
      case 2:
        stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
        break;
      case 3:
        stage = vk::ShaderStageFlagBits::eGeometry;
      default:
        stage = vk::ShaderStageFlagBits::eFragment;
    }
    
    if (!path.has_value()) continue;
    
    auto shader = readShader(path.value());
    vk::ShaderModuleCreateInfo ci_shaderModule{
      .codeSize = shader.size(),
      .pCode    = reinterpret_cast<const unsigned int *>(shader.data())
    };
    modules.emplace_back(
      std::pair<vk::ShaderStageFlagBits, vk::raii::ShaderModule>(stage, vk_device.createShaderModule(ci_shaderModule))
    );
  } 

  return modules;
}

ShaderInfos GraphicsComponent::shaderInfos(const ShaderModules& modules) const
{
  ShaderInfos createInfos;

  for (const auto& module : modules)
  {
    vk::PipelineShaderStageCreateInfo ci_shaderStage{
      .stage  = module.first,
      .module = *module.second,
      .pName  = "main"
    };
    createInfos.emplace_back(ci_shaderStage);
  }

  return createInfos;
}

void GraphicsComponent::createPipeline(const vk::raii::Device& vk_device)
{
  auto vk_shaderModules = createShaderModules(vk_device);
  auto ci_shaderStages = shaderInfos(vk_shaderModules);

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
  auto vk_attributeDescription = Vertex::attributeDescription();
  vk::PipelineVertexInputStateCreateInfo ci_vertexInput{
    .vertexBindingDescriptionCount    = 1,
    .pVertexBindingDescriptions       = &vk_bindingDescription,
    .vertexAttributeDescriptionCount  = 1,
    .pVertexAttributeDescriptions     = &vk_attributeDescription
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
    .frontFace                = vk::FrontFace::eClockwise,
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

  vk::DescriptorSetLayoutBinding globalBinding{
    .binding          = 0,
    .descriptorType   = vk::DescriptorType::eUniformBuffer,
    .descriptorCount  = 1,
    .stageFlags       = vk::ShaderStageFlagBits::eVertex
  };

  vk::DescriptorSetLayoutCreateInfo ci_globalLayout{
    .bindingCount = 1,
    .pBindings    = &globalBinding
  };

  vk_descriptorLayouts.emplace_back(vk_device.createDescriptorSetLayout(ci_globalLayout));

  vk::PushConstantRange modelPush{
    .stageFlags = vk::ShaderStageFlagBits::eVertex,
    .offset     = 0,
    .size       = sizeof(glm::mat4)
  };
  
  std::vector<vk::DescriptorSetLayout> descriptorLayouts;
  for (auto& vk_descriptorLayout : vk_descriptorLayouts)
    descriptorLayouts.emplace_back(*vk_descriptorLayout);

  vk::PipelineLayoutCreateInfo ci_pipelineLayout{
    .setLayoutCount         = static_cast<unsigned int>(descriptorLayouts.size()),
    .pSetLayouts            = descriptorLayouts.data(),
    .pushConstantRangeCount = 1,
    .pPushConstantRanges    = &modelPush
  };
  vk_pipelineLayout = vk_device.createPipelineLayout(ci_pipelineLayout);

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
    .layout               = *vk_pipelineLayout
  };

  vk_pipeline = vk_device.createGraphicsPipeline(nullptr, ci_pipeline);
}

void GraphicsComponent::allocateBuffers(const Device& device)
{
  vk::DeviceSize vertexSize = sizeof(Vertex) * vertices.size();
  vk::DeviceSize indexSize = sizeof(unsigned int) * indices.size();
  
  vk::BufferCreateInfo ci_vertexStaging{
    .size         = vertexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  auto vertexStagingBuffer = device.logical().createBuffer(ci_vertexStaging);

  vk::BufferCreateInfo ci_indexStaging{
    .size         = indexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  auto indexStagingBuffer = device.logical().createBuffer(ci_indexStaging);

  auto vertexRequirements = vertexStagingBuffer.getMemoryRequirements();
  auto indexRequirements = indexStagingBuffer.getMemoryRequirements();
  
  vk::DeviceSize offset = vertexRequirements.size;
  while (offset % indexRequirements.alignment != 0)
    ++offset;

  vk::MemoryAllocateInfo i_stagingMemory{
    .allocationSize   = offset + indexRequirements.size,
    .memoryTypeIndex  = findMemoryIndex(device.physical(),
                                        vertexRequirements.memoryTypeBits | indexRequirements.memoryTypeBits,
                                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
  };
  auto stagingMemory = device.logical().allocateMemory(i_stagingMemory);

  vertexStagingBuffer.bindMemory(*stagingMemory, 0);
  indexStagingBuffer.bindMemory(*stagingMemory, offset);

  void * map = stagingMemory.mapMemory(0, vertexSize);
  memcpy(map, vertices.data(), vertexSize);
  stagingMemory.unmapMemory();

  map = stagingMemory.mapMemory(offset, indexSize);
  memcpy(map, indices.data(), indexSize);
  stagingMemory.unmapMemory();

  vk::BufferCreateInfo ci_vertexBuffer{
    .size   = vertexSize,
    .usage  = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_vertexBuffer = device.logical().createBuffer(ci_vertexBuffer);

  vk::BufferCreateInfo ci_indexBuffer{
    .size         = indexSize,
    .usage        = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_indexBuffer = device.logical().createBuffer(ci_indexBuffer);

  vertexRequirements = vk_vertexBuffer.getMemoryRequirements();
  indexRequirements = vk_indexBuffer.getMemoryRequirements();

  offset = vertexRequirements.size;
  while (offset % indexRequirements.alignment != 0)
    ++offset;
  
  vk::MemoryAllocateInfo i_objectMemory{
    .allocationSize   = indexRequirements.size + offset,
    .memoryTypeIndex  = findMemoryIndex(device.physical(),
                                        vertexRequirements.memoryTypeBits | indexRequirements.memoryTypeBits,
                                        vk::MemoryPropertyFlagBits::eDeviceLocal)
  };
  vk_objectMemory = device.logical().allocateMemory(i_objectMemory);

  vk_vertexBuffer.bindMemory(*vk_objectMemory, 0);
  vk_indexBuffer.bindMemory(*vk_objectMemory, offset);
  
  FamilyType familyType;
  if (device.hasFamily(FamilyType::Transfer))
    familyType = FamilyType::Transfer;
  else if (device.hasFamily(FamilyType::AsyncCombine))
    familyType = FamilyType::AsyncCombine;
  else
    familyType = FamilyType::AllQueue;

  vk::CommandPoolCreateInfo ci_transferPool{
    .flags  = vk::CommandPoolCreateFlagBits::eTransient,
    .queueFamilyIndex = static_cast<unsigned int>(device.familyIndex(familyType))
  };
  auto vk_transferPool = device.logical().createCommandPool(ci_transferPool);

  vk::CommandBufferAllocateInfo i_transferBuffers{
    .commandPool        = *vk_transferPool,
    .commandBufferCount = 2
  };
  auto vk_transferBuffers = device.logical().allocateCommandBuffers(i_transferBuffers);

  vk::CommandBufferBeginInfo bi_transferBuffer;
  
  for (auto& vk_transferBuffer : vk_transferBuffers)
    vk_transferBuffer.begin(bi_transferBuffer);

  vk::BufferCopy vertexCopy{
    .srcOffset  = 0,
    .dstOffset  = 0,
    .size       = vertexSize
  };

  vk::BufferCopy indexCopy{
    .srcOffset  = 0,
    .dstOffset  = 0,
    .size       = indexSize
  };

  vk_transferBuffers[0].copyBuffer(*vertexStagingBuffer, *vk_vertexBuffer, vertexCopy);
  vk_transferBuffers[1].copyBuffer(*indexStagingBuffer, *vk_indexBuffer, indexCopy);

  for (auto& vk_transferBuffer : vk_transferBuffers)
    vk_transferBuffer.end();

  vk::FenceCreateInfo ci_transferFence;
  auto vk_transferFence = device.logical().createFence(ci_transferFence);

  std::array<vk::CommandBuffer, 2> transferBuffers = { *vk_transferBuffers[0], *vk_transferBuffers[1] };
  
  vk::SubmitInfo i_transferSubmit{
    .commandBufferCount = static_cast<unsigned int>(transferBuffers.size()),
    .pCommandBuffers = transferBuffers.data()
  };

  device.queue(familyType).submit(i_transferSubmit, *vk_transferFence);

  static_cast<void>(device.logical().waitForFences(*vk_transferFence, vk::True, UINT64_MAX));
}

void GraphicsComponent::createDescriptorSets(const vk::raii::Device& vk_device, unsigned long maxFlightFrames, const vk::raii::Buffer& vk_cameraBuffer)
{
  vk::DescriptorPoolSize poolSize{
    .type             = vk::DescriptorType::eUniformBuffer,
    .descriptorCount  = static_cast<unsigned int>(maxFlightFrames)
  };

  vk::DescriptorPoolCreateInfo ci_descriptorPool{
    .flags          = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    .maxSets        = static_cast<unsigned int>(maxFlightFrames),
    .poolSizeCount  = 1,
    .pPoolSizes     = &poolSize
  };

  vk_descriptorPool = vk_device.createDescriptorPool(ci_descriptorPool);

  for (unsigned long i = 0; i < maxFlightFrames; ++i)
  {
    vk::DescriptorSetAllocateInfo i_globalSet{
      .descriptorPool     = *vk_descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts        = &*vk_descriptorLayouts[0]
    };

    vk_descriptorSets.emplace_back(vk_device.allocateDescriptorSets(i_globalSet));

    vk::DescriptorBufferInfo cameraInfo{
      .buffer = *vk_cameraBuffer,
      .offset = 0,
      .range  = sizeof(Camera)
    };
    
    vk::WriteDescriptorSet globalWrite{
      .dstSet           = *vk_descriptorSets[i][0],
      .dstBinding       = 0,
      .dstArrayElement  = 0,
      .descriptorCount  = 1,
      .descriptorType   = vk::DescriptorType::eUniformBuffer,
      .pBufferInfo      = &cameraInfo
    };

    vk_device.updateDescriptorSets({ globalWrite }, nullptr);
  }
}

} // namespace pecs 