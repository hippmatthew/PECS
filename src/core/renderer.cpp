#include "src/core/include/renderer.hpp"

#define VECS_USE_STAGING          "staging"
#define VECS_USE_DEVICE           "device"

#define VECS_FLIGHT_FENCE_TAG     "flight"
#define VECS_IMAGE_SEMAPHORE_TAG  "image"
#define VECS_RENDER_SEMAPHORE_TAG "render"

namespace vecs
{

void Renderer::load(const Device& device)
{
  unsigned long maxBuffers = 0;
  for (auto& entity : EntityController::instance().entities())
  {
    if (!(entity->id() & VECS_RENDERER_SYSTEM)) continue;

    std::shared_ptr<GraphicsComponent> p_entity = std::make_shared<GraphicsComponent>(*entity);
    
    if (maxBuffers < 2 + p_entity->uniformMap.size())
      maxBuffers = 2 + p_entity->uniformMap.size();
    
    entities.emplace_back(p_entity);
  }
  ++maxBuffers;

  createCommands(device, maxBuffers);

  vk::BufferCreateInfo ci_cameraBuffer{
    .size         = sizeof(Camera),
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_cameraTransfer = device.logical().createBuffer(ci_cameraBuffer);

  ci_cameraBuffer.usage = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferSrc;
  vk_cameraDevice = device.logical().createBuffer(ci_cameraBuffer);
  
  for (const auto& entity : entities)
    createBuffers(device, entity);
  allocateMemory(device);

  unsigned long index = 0;
  for (const auto& entity : entities)
  {
    allocateDescriptors(device, entity, index);
    loadModels(device, entity, index);
    ++index;
  }

  initializeSyncObjects();
}

void Renderer::render(const Device& device, GUI& gui)
{
  auto result = gui.swapchain().acquireNextImage(UINT64_MAX, *Synchronization::instance().semaphore(VECS_IMAGE_SEMAPHORE_TAG + std::to_string(frameIndex)), nullptr);
  if (result.first == vk::Result::eErrorOutOfDateKHR || result.first == vk::Result::eSuboptimalKHR)
  {
    gui.recreateSwapchain(device);
  }
  else if (result.first != vk::Result::eSuccess)
    throw std::runtime_error("error @ vecs::Renderer::render() : failed to get next image");

  auto imageIndex = result.second;
  auto vk_image = gui.image(imageIndex);
  auto vk_imageView = *gui.imageView(imageIndex);

  vk_renderCommands[frameIndex].reset();

  vk::CommandBufferBeginInfo bi_commandBuffer;
  vk_renderCommands[frameIndex].begin(bi_commandBuffer);

  vk::ImageMemoryBarrier memoryBarrier{
    .dstAccessMask    = vk::AccessFlagBits::eColorAttachmentWrite,
    .oldLayout        = vk::ImageLayout::eUndefined,
    .newLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    .image            = vk_image,
    .subresourceRange = {
      .aspectMask       = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel     = 0,
      .levelCount       = 1,
      .baseArrayLayer   = 0,
      .layerCount       = 1
    }
  };

  vk_renderCommands[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eTopOfPipe,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    memoryBarrier
  );

  vk::RenderingAttachmentInfo i_attachment{
    .imageView    = vk_imageView,
    .imageLayout  = vk::ImageLayout::eColorAttachmentOptimal,
    .loadOp       = vk::AttachmentLoadOp::eClear,
    .storeOp      = vk::AttachmentStoreOp::eStore,
    .clearValue   = Settings::instance().background_color()
  };

  vk::RenderingInfo i_rendering{
    .renderArea           = { .offset = { 0, 0 },
                              .extent = Settings::instance().extent() },
    .layerCount           = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &i_attachment
  };

  vk_renderCommands[frameIndex].beginRenderingKHR(i_rendering);

  vk::Viewport vk_viewport{
    .x = 0.0f,
    .y = 0.0f,
    .width = static_cast<float>(Settings::instance().extent().width),
    .height = static_cast<float>(Settings::instance().extent().height),
    .minDepth = 0.0f,
    .maxDepth = 0.0f
  };
  vk_renderCommands[frameIndex].setViewport(0, vk_viewport);

  vk::Rect2D vk_scissor{
    .offset = {0, 0},
    .extent = Settings::instance().extent()
  };
  vk_renderCommands[frameIndex].setScissor(0, vk_scissor);
  
  unsigned long entityIndex = 0;
  for (const auto& entity : entities)
  {
    updateBuffers(device, entity, entityIndex);

    vk_renderCommands[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *(pipelineMap.at(entity->material.tag)));

    unsigned long setCount = vk_descriptorSets[entityIndex].size() / Settings::instance().max_flight_frames();
    std::vector<vk::DescriptorSet> sets;
    for (unsigned long i = 0; i < setCount; ++i)
      sets.emplace_back(*(vk_descriptorSets[entityIndex][frameIndex * setCount + i]));

    vk_renderCommands[frameIndex].bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      *(pipelineLayoutMap.at(entity->material.tag)),
      0,
      vk::ArrayProxy<vk::DescriptorSet>(sets.size(), sets.data()),
      nullptr
    );

    vk_renderCommands[frameIndex].pushConstants<glm::mat4>(
      *(pipelineLayoutMap.at(entity->material.tag)),
      vk::ShaderStageFlagBits::eVertex,
      0,
      entity->model
    );

    for (const auto& tag : entity->pushConstantTags)
      entity->pushConstantMap.at(tag)->bindPushConstant(vk_renderCommands[frameIndex], *(pipelineLayoutMap.at(entity->material.tag)));

    vk_renderCommands[frameIndex].bindVertexBuffers(0, { *(vk_deviceBuffers[entityIndex][0]) }, { 0 });
    vk_renderCommands[frameIndex].bindIndexBuffer(*(vk_deviceBuffers[entityIndex][1]), 0, vk::IndexType::eUint32);

    vk_renderCommands[frameIndex].drawIndexed(entity->indices.size(), 1, 0, 0, 0);
  }

  vk_renderCommands[frameIndex].endRendering();
  
  vk::ImageMemoryBarrier memoryBarrier{
    .srcAccessMask    = vk::AccessFlagBits::eColorAttachmentWrite,
    .oldLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    .newLayout        = vk::ImageLayout::ePresentSrcKHR,
    .image            = vk_image,
    .subresourceRange = {
      .aspectMask       = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel     = 0,
      .levelCount       = 1,
      .baseArrayLayer   = 0,
      .layerCount       = 1
    }
  };
  
  vk_renderCommands[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    memoryBarrier
  );

  vk_renderCommands[frameIndex].end();

  vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
  vk::SubmitInfo rendererSubmit{
    .waitSemaphoreCount   = 1,
    .pWaitSemaphores      = &*Synchronization::instance().semaphore(VECS_IMAGE_SEMAPHORE_TAG + std::to_string(frameIndex)),
    .pWaitDstStageMask    = &waitStages,
    .commandBufferCount   = 1,
    .pCommandBuffers      = &*vk_renderCommands[frameIndex],
    .signalSemaphoreCount = 1,
    .pSignalSemaphores    = &*Synchronization::instance().semaphore(VECS_RENDER_SEMAPHORE_TAG + std::to_string(frameIndex))
  };

  device.queue(FamilyType::All).submit(rendererSubmit, *Synchronization::instance().fence(VECS_FLIGHT_FENCE_TAG + std::to_string(frameIndex)));

  vk::PresentInfoKHR presentInfo{
    .waitSemaphoreCount = 1,
    .pWaitSemaphores    = &*Synchronization::instance().semaphore(VECS_RENDER_SEMAPHORE_TAG + std::to_string(frameIndex)),
    .swapchainCount     = 1,
    .pSwapchains        = &*gui.swapchain(),
    .pImageIndices      = &imageIndex
  };

  auto presentResult = device.queue(FamilyType::All).presentKHR(presentInfo);

  if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
    gui.recreateSwapchain(device);
  else if (presentResult != vk::Result::eSuccess)
    throw std::runtime_error("error @ vecs::Renderer::render() : failed to present image");

  frameIndex = (frameIndex + 1) % Settings::instance().max_flight_frames();
}

unsigned int Renderer::findIndex(const vk::raii::PhysicalDevice& vk_physicalDevice, unsigned int bits, vk::MemoryPropertyFlags flags) const
{
  auto memoryProperties = vk_physicalDevice.getMemoryProperties();
  
  for (unsigned long i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    if ((bits & (1 << i)) &&
        (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
    {
      return i;
    }
  }
  
  throw std::runtime_error("error @ pecs::PipelineComponent::findMemeoryIndex() : could not find suitable memory index");
}

void Renderer::initializeSyncObjects() const
{
  for (unsigned long i = 0; i < Settings::instance().max_flight_frames(); ++i)
  {
    Synchronization::instance().add_fence(VECS_FLIGHT_FENCE_TAG + std::to_string(i), true);
    Synchronization::instance().add_semaphore(VECS_IMAGE_SEMAPHORE_TAG + std::to_string(i));
    Synchronization::instance().add_semaphore(VECS_RENDER_SEMAPHORE_TAG + std::to_string(i));
  }
};

void Renderer::createCommands(const Device& device, unsigned long maxBuffers)
{
  unsigned long index = device.familyIndex(FamilyType::All);
  if (device.hasFamily(FamilyType::Transfer))
    index = device.familyIndex(FamilyType::Transfer);
  else if (device.hasFamily(FamilyType::Async))
    index = device.familyIndex(FamilyType::Async);
  
  vk::CommandPoolCreateInfo ci_renderPool{
    .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = device.familyIndex(FamilyType::All)
  };
  vk_renderPool = device.logical().createCommandPool(ci_renderPool);

  vk::CommandBufferAllocateInfo ai_renderBuffers{
    .commandPool  = *vk_renderPool,
    .level        = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = static_cast<unsigned int>(Settings::instance().max_flight_frames())
  };
  for (auto& buffer : device.logical().allocateCommandBuffers(ai_renderBuffers))
    vk_renderCommands.emplace_back(std::move(buffer));

  vk::CommandPoolCreateInfo ci_transferPool{
    .flags            = vk::CommandPoolCreateFlagBits::eTransient,
    .queueFamilyIndex = index
  };
  vk_transferPool = device.logical().createCommandPool(ci_transferPool);

  vk::CommandBufferAllocateInfo ai_transferBuffers{
    .commandPool        = *vk_transferPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = static_cast<unsigned int>(maxBuffers)
  };
  
  for (auto& buffer : device.logical().allocateCommandBuffers(ai_transferBuffers))
    vk_transferCommands.emplace_back(std::move(buffer));
}

void Renderer::createBuffers(const Device& device, const std::shared_ptr<GraphicsComponent>& entity)
{
  std::vector<vk::raii::Buffer> transferBuffers, deviceBuffers;
    
  vk::DeviceSize vertexSize = sizeof(VertexData) * entity->vertices.size();
  vk::DeviceSize indexSize = sizeof(unsigned int) * entity->indices.size();
    
  vk::BufferCreateInfo ci_vertexBuffer{
    .size         = vertexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  transferBuffers.emplace_back(device.logical().createBuffer(ci_vertexBuffer));

  vk::BufferCreateInfo ci_indexBuffer{
    .size         = indexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  transferBuffers.emplace_back(device.logical().createBuffer(ci_indexBuffer));

  unsigned long index = 2;
  for (const auto& tag : entity->uniformTags)
  {
    vk::BufferCreateInfo ci_uniformBuffer{
      .size         = entity->uniformMap[tag]->size(),
      .usage        = vk::BufferUsageFlagBits::eTransferSrc,
      .sharingMode  = vk::SharingMode::eExclusive
    };
    transferBuffers.emplace_back(device.logical().createBuffer(ci_uniformBuffer));

    entity->uniformMap[tag]->index = index++;
  }

  ci_vertexBuffer.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
  deviceBuffers.emplace_back(device.logical().createBuffer(ci_vertexBuffer));

  ci_indexBuffer.usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
  deviceBuffers.emplace_back(device.logical().createBuffer(ci_indexBuffer));

  for (const auto& tag : entity->uniformTags)
  {
    vk::BufferCreateInfo ci_uniformBuffer{
      .size         = entity->uniformMap[tag]->size(),
      .usage        = vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst,
      .sharingMode  = vk::SharingMode::eExclusive
    };
    deviceBuffers.emplace_back(device.logical().createBuffer(ci_uniformBuffer));
  }

  vk_transferBuffers.emplace_back(transferBuffers);
  vk_deviceBuffers.emplace_back(deviceBuffers);
}

void Renderer::allocateMemory(const Device& device)
{
  vk::DeviceSize size = vk_cameraTransfer.getMemoryRequirements().size;
  
  unsigned int bits = 0x0u;
  for (const auto& buffers : vk_transferBuffers)
  {
    for (const auto& buffer : buffers)
    {  
      auto requirements = buffer.getMemoryRequirements();
      while (size % requirements.alignment != 0)
        ++size;
      
      size += requirements.size;
      bits |= requirements.memoryTypeBits;
    }
  }

  vk::MemoryAllocateInfo ai_staging{
    .allocationSize   = size,
    .memoryTypeIndex  = findIndex(device.physical(), bits, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible)
  };
  vk_stagingMemory = device.logical().allocateMemory(ai_staging);

  vk_cameraTransfer.bindMemory(*vk_stagingMemory, 0);
  vk::DeviceSize offset = vk_cameraTransfer.getMemoryRequirements().size;

  unsigned long entityIndex = 0;
  for (const auto& buffers : vk_transferBuffers)
  {
    for (const auto& buffer : buffers)
    {
      stagingOffsets[entityIndex].emplace_back(offset);

      auto requirements = buffer.getMemoryRequirements();
      while (offset % requirements.alignment != 0)
        ++offset;

      buffer.bindMemory(*vk_stagingMemory, offset);
      offset += requirements.size;
    }
    ++entityIndex;
  }
  
  size = vk_cameraDevice.getMemoryRequirements().size;

  for (const auto& buffers : vk_deviceBuffers)
  {
    for (const auto& buffer : buffers)
    {
      auto requirements = buffer.getMemoryRequirements();
      while (size % requirements.alignment != 0)
        ++size;
      
      size += requirements.size;
      bits |= requirements.memoryTypeBits;
    }
  }

  vk::MemoryAllocateInfo ai_device{
    .allocationSize   = size,
    .memoryTypeIndex  = findIndex(device.physical(), bits, vk::MemoryPropertyFlagBits::eDeviceLocal)
  };
  vk_deviceMemory = device.logical().allocateMemory(ai_device);

  vk_cameraDevice.bindMemory(*vk_deviceMemory, 0);
  offset = vk_cameraDevice.getMemoryRequirements().size;

  deviceOffsets.emplace_back(offset);
  for (const auto& buffers : vk_deviceBuffers)
  {
    for (const auto& buffer : buffers)
    {
      auto requirements = buffer.getMemoryRequirements();
      while (offset % requirements.alignment != 0)
        ++offset;

      buffer.bindMemory(*vk_deviceMemory, offset);
      offset += requirements.size;
    }
    deviceOffsets.emplace_back(offset);
  }
}

void Renderer::addPipeline(const Device& device, const std::shared_ptr<GraphicsComponent>& entity)
{
  std::vector<vk::raii::ShaderModule> shaderModules;
  std::vector<vk::PipelineShaderStageCreateInfo> ci_shaders;

  unsigned long index = 0;
  for (const auto& shader : entity->material.shaders())
  {
    vk::ShaderStageFlagBits stage;
    switch (shader)
    {
      case Vertex:
        stage = vk::ShaderStageFlagBits::eVertex;
        break;
      case Tesselation1:
        stage = vk::ShaderStageFlagBits::eTessellationControl;
        break;
      case Tesselation2:
        stage = vk::ShaderStageFlagBits::eTessellationEvaluation;
        break;
      case Geometry:
        stage = vk::ShaderStageFlagBits::eGeometry;
        break;
      case Fragment:
        stage = vk::ShaderStageFlagBits::eFragment;
        break;
      case sCompute:
        continue;
    }

    auto binary = entity->material.binary(shader);
    
    vk::ShaderModuleCreateInfo ci_shaderModule{
      .codeSize = binary.size(),
      .pCode    = reinterpret_cast<const unsigned int *>(binary.data())
    };
    shaderModules.emplace_back(device.logical().createShaderModule(ci_shaderModule));

    vk::PipelineShaderStageCreateInfo ci_shader{
      .stage  = stage,
      .module = shaderModules[index++],
      .pName  = "main"
    };
    ci_shaders.emplace_back(ci_shader);
  }

  std::vector<vk::DynamicState> dynamicStates{
    vk::DynamicState::eViewport,
    vk::DynamicState::eScissor
  };

  vk::PipelineDynamicStateCreateInfo ci_dynamicStates{
    .dynamicStateCount  = static_cast<unsigned int>(dynamicStates.size()),
    .pDynamicStates     = dynamicStates.data()
  };

  vk::PipelineViewportStateCreateInfo ci_viewportState{
    .viewportCount  = 1,
    .scissorCount   = 1
  };

  auto binding = VertexData::binding();
  auto attribute = VertexData::attribute();

  vk::PipelineVertexInputStateCreateInfo ci_vertexInput{
    .vertexBindingDescriptionCount    = 1,
    .pVertexBindingDescriptions       = &binding,
    .vertexAttributeDescriptionCount  = 1,
    .pVertexAttributeDescriptions     = &attribute
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

  std::vector<vk::raii::DescriptorSetLayout> setLayouts{
    device.logical().createDescriptorSetLayout(ci_globalLayout)
  };

  if (!entity->uniformTags.empty())
  {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    for (unsigned long i = 0; i < entity->uniformTags.size(); ++i)
    {
      vk::DescriptorSetLayoutBinding uniformBinding{
        .binding          = i,
        .descriptorType   = vk::DescriptorType::eUniformBuffer,
        .descriptorCount  = 1,
        .stageFlags       = vk::ShaderStageFlagBits::eAllGraphics
      };
      bindings.emplace_back(uniformBinding);
    }

    vk::DescriptorSetLayoutCreateInfo ci_uniformLayout{
      .bindingCount = static_cast<unsigned int>(bindings.size()),
      .pBindings    = bindings.data()
    };
    setLayouts.emplace_back(device.logical().createDescriptorSetLayout(ci_uniformLayout));
  }

  descriptorLayoutMap.emplace(std::make_pair(entity->material.tag, setLayouts));
  
  vk::PushConstantRange modelPush{
    .stageFlags = vk::ShaderStageFlagBits::eVertex,
    .offset     = 0,
    .size       = sizeof(glm::mat4)
  };

  std::vector<vk::PushConstantRange> pushRanges{ modelPush };

  if (!entity->pushConstantTags.empty())
  {
    unsigned int offset = sizeof(glm::mat4);

    for (const auto& tag : entity->pushConstantTags)
    {
      unsigned int size = entity->pushConstantMap[tag]->size();
      
      vk::PushConstantRange customPush{
      .stageFlags = vk::ShaderStageFlagBits::eAll,
      .offset     = offset,
      .size       = size
      };
      pushRanges.emplace_back(customPush);

      offset += size;
    }
  }

  std::vector<vk::DescriptorSetLayout> layouts;
  for (const auto& layout : setLayouts)
    layouts.emplace_back(*layout);

  vk::PipelineLayoutCreateInfo ci_layout{
    .setLayoutCount         = static_cast<unsigned int>(layouts.size()),
    .pSetLayouts            = layouts.data(),
    .pushConstantRangeCount = static_cast<unsigned int>(pushRanges.size()),
    .pPushConstantRanges    = pushRanges.data()
  };
  pipelineLayoutMap.emplace(std::make_pair(entity->material.tag, device.logical().createPipelineLayout(ci_layout)));

  vk::Format format = Settings::instance().format();
  vk::PipelineRenderingCreateInfo ci_rendering{
    .colorAttachmentCount     = 1,
    .pColorAttachmentFormats  = &format
  };

  vk::GraphicsPipelineCreateInfo ci_pipeline{
    .pNext                = &ci_rendering,
    .stageCount           = static_cast<unsigned int>(ci_shaders.size()),
    .pStages              = ci_shaders.data(),
    .pVertexInputState    = &ci_vertexInput,
    .pInputAssemblyState  = &ci_inputAssembly,
    .pViewportState       = &ci_viewportState,
    .pRasterizationState  = &ci_rasterizer,
    .pMultisampleState    = &ci_multisampling,
    .pDepthStencilState   = nullptr,
    .pColorBlendState     = &ci_blendState,
    .pDynamicState        = &ci_dynamicStates,
    .layout               = *(pipelineLayoutMap[entity->material.tag])
  };

  pipelineMap.emplace(std::make_pair(entity->material.tag, device.logical().createGraphicsPipeline(nullptr, ci_pipeline)));
}

void Renderer::allocatePools(const Device& device)
{
  unsigned int maxSets = 0;
  std::vector<vk::DescriptorPoolSize> poolSizes;

  for (const auto& entity : entities)
  {
    if (pipelineMap.find(entity->material.tag) == pipelineMap.end())
      addPipeline(device, entity);

    unsigned int sets = Settings::instance().max_flight_frames() * (1 + entity->uniformTags.size());
    maxSets += sets;

    vk::DescriptorPoolSize poolSize{
      .type             = vk::DescriptorType::eUniformBuffer,
      .descriptorCount  = sets
    };
    poolSizes.emplace_back(poolSize);
  }

  vk::DescriptorPoolCreateInfo ci_pool{
    .flags          = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    .maxSets        = maxSets,
    .poolSizeCount  = static_cast<unsigned int>(poolSizes.size()),
    .pPoolSizes     = poolSizes.data()
  };
  vk_descriptorPool = device.logical().createDescriptorPool(ci_pool);
}

void Renderer::allocateDescriptors(const Device& device, const std::shared_ptr<GraphicsComponent>& entity, unsigned long index)
{
  std::vector<vk::WriteDescriptorSet> writes;
  
  unsigned long sets = 0;
  for (unsigned long i = 0; i < Settings::instance().max_flight_frames(); ++i)
  {
    std::vector<vk::DescriptorSetLayout> layouts;
    for (const auto& layout : descriptorLayoutMap[entity->material.tag])
      layouts.emplace_back(*layout);
    
    vk::DescriptorSetAllocateInfo ai_sets{
      .descriptorPool     = *vk_descriptorPool,
      .descriptorSetCount = static_cast<unsigned int>(layouts.size()),
      .pSetLayouts        = layouts.data()
    };

    vk_descriptorSets[index].emplace_back(device.logical().allocateDescriptorSets(ai_sets));
    (sets == 0) ? sets = vk_descriptorSets[index].size() / Settings::instance().max_flight_frames() : sets == sets;

    vk::DescriptorBufferInfo globalInfo{
      .buffer = *vk_cameraDevice,
      .offset = 0,
      .range  = sizeof(Camera)
    };

    vk::WriteDescriptorSet globalWrite{
      .dstSet           = *vk_descriptorSets[index][i * sets],
      .dstBinding       = 0,
      .dstArrayElement  = 0,
      .descriptorCount  = 1,
      .descriptorType   = vk::DescriptorType::eUniformBuffer,
      .pBufferInfo      = &globalInfo
    };

    writes.emplace_back(globalWrite);
  }
  
  unsigned long i = 1;
  for (const auto& tag : entity->uniformTags)
  {
    vk::DescriptorBufferInfo info{
      .buffer = *vk_deviceBuffers[index][i + 1],
      .offset = 0,
      .range  = entity->uniformMap[tag]->size()
    };

    for (unsigned long j = 0; j < Settings::instance().max_flight_frames(); ++j)
    {
      vk::WriteDescriptorSet write{
        .dstSet           = *vk_descriptorSets[index][i + (j * sets)],
        .dstBinding       = i - 1,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo      = &info
      };

      writes.emplace_back(write);
    }
  }

  device.logical().updateDescriptorSets(vk::ArrayProxy<vk::WriteDescriptorSet>(writes.size(), writes.data()), nullptr);
}

void Renderer::loadModels(const Device& device, const std::shared_ptr<GraphicsComponent>& entity, unsigned long index)
{
  vk::DeviceSize vertexSize = sizeof(VertexData) * entity->vertices.size();
  vk::DeviceSize indexSize = sizeof(unsigned int) * entity->indices.size();
  
  void * data = vk_stagingMemory.mapMemory(stagingOffsets[index][0], vertexSize);
  memcpy(data, entity->vertices.data(), vertexSize);
  vk_stagingMemory.unmapMemory();

  data = vk_stagingMemory.mapMemory(stagingOffsets[index][1], indexSize);
  memcpy(data, entity->indices.data(), indexSize);
  vk_stagingMemory.unmapMemory();

  vk::CommandBufferBeginInfo beginInfo;

  vk_transferCommands[2 * index].begin(beginInfo);
  vk_transferCommands[2 * index + 1].begin(beginInfo);

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

  vk_transferCommands[2 * index].copyBuffer(*vk_transferBuffers[index][0], *vk_deviceBuffers[index][0], vertexCopy);
  vk_transferCommands[2 * index + 1].copyBuffer(*vk_transferBuffers[index][1], *vk_deviceBuffers[index][1], indexCopy);

  std::vector<vk::CommandBuffer> transferCommands = { *vk_transferCommands[2 * index], *vk_transferCommands[2 * index + 1] };

  vk::SubmitInfo transferSubmission{
    .commandBufferCount = static_cast<unsigned int>(transferCommands.size()),
    .pCommandBuffers    = transferCommands.data()
  };

  FamilyType type = FamilyType::All;
  if (device.hasFamily(FamilyType::Transfer))
    type = FamilyType::Transfer;
  else if (device.hasFamily(FamilyType::Async))
    type = FamilyType::Async;

  Synchronization::instance().add_fence("transfer");

  device.queue(type).submit(transferSubmission, *(Synchronization::instance().fence("transfer")));

  Synchronization::instance().wait_fences({ "transfer" });
  Synchronization::instance().remove_fence("transfer");
}

void Renderer::updateBuffers(const Device& device, const std::shared_ptr<GraphicsComponent>& entity, unsigned long entityIndex)
{
  std::vector<vk::CommandBuffer> transferCommands;
  
  unsigned long bufferIndex = 0;
  for (const auto& tag : entity->uniformTags)
  {
    if (bufferIndex < 2)
    {
      ++bufferIndex;
      continue;
    }

    vk::CommandBufferBeginInfo beginInfo;
    vk_transferCommands[bufferIndex - 2].begin(beginInfo);

    vk::BufferCopy copy{
      .srcOffset  = 0,
      .dstOffset  = 0,
      .size       = entity->uniformMap.at(tag)->size()
    };

    vk_transferCommands[bufferIndex - 2].copyBuffer(*(vk_transferBuffers[entityIndex][bufferIndex]), *(vk_deviceBuffers[entityIndex][bufferIndex]), copy);
    transferCommands.emplace_back(*vk_transferCommands[bufferIndex - 2]);

    ++bufferIndex;
  }

  if (transferCommands.empty()) return;

  vk::SubmitInfo transferSubmission{
    .commandBufferCount = static_cast<unsigned int>(transferCommands.size()),
    .pCommandBuffers    = transferCommands.data()
  };

  FamilyType type = FamilyType::All;
  if (device.hasFamily(FamilyType::Transfer))
    type = FamilyType::Transfer;
  else if (device.hasFamily(FamilyType::Async))
    type = FamilyType::Async;

  Synchronization::instance().add_fence("transfer");

  device.queue(type).submit(transferSubmission, { *(Synchronization::instance().fence("transfer")) });

  Synchronization::instance().wait_fences({ "transfer" });
  Synchronization::instance().remove_fence("transfer");
}

} // namespace vecs