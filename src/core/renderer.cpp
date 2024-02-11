/*
 *  PECS::core - renderer.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/4/24
 *  Updated:  2/9/24
 */

#include "src/core/include/renderer.hpp"

namespace pecs
{

Renderer::Renderer(const Settings::Renderer& s, const Device& device, const ViewportInfo& vi) : settings(s)
{
  createCommandPools(device.logical(), device.queueFamilyArray());
  createRenderBuffers(device.logical());

  i_viewport = vi;
  renderArea.offset = {0, 0};
  renderArea.extent = i_viewport.first;
}

const std::vector<vk::raii::CommandBuffer>& Renderer::commandBuffers() const
{
  return vk_renderBuffers;
}

unsigned int Renderer::maxFlightFrames() const
{
  return settings.maxFlightFrames;
}

void Renderer::render(std::vector<Object *>& objects, const unsigned int& frameIndex, const vk::raii::Image& vk_image, const vk::raii::ImageView& vk_imageView, const Device& device)
{    
  vk_renderBuffers[frameIndex].reset();
  
  beginRendering(frameIndex, vk_image, vk_imageView);

  vk::Viewport vk_viewport{
    .x = 0.0f,
    .y = 0.0f,
    .width = static_cast<float>(i_viewport.first.width),
    .height = static_cast<float>(i_viewport.first.height),
    .minDepth = 0.0f,
    .maxDepth = 0.0f
  };
  vk_renderBuffers[frameIndex].setViewport(0, vk_viewport);

  vk::Rect2D vk_scissor{
    .offset = {0, 0},
    .extent = i_viewport.first
  };
  vk_renderBuffers[frameIndex].setScissor(0, vk_scissor);

  std::size_t bufferIndex = 0;
  for (auto * object : objects)
  {
    vk_renderBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *object->vk_graphicsPipeline);
    vk_renderBuffers[frameIndex].bindVertexBuffers(0, { *vk_vertexBuffers[bufferIndex] }, { 0 });

    vk_renderBuffers[frameIndex].draw(object->vertices.size(), 1, 0, 0);
    
    ++bufferIndex;
  }

  endRendering(frameIndex, vk_image);
}

unsigned int Renderer::findMemoryIndex(const vk::raii::PhysicalDevice& vk_physicalDevice, unsigned int filter, vk::MemoryPropertyFlags vk_memoryProperties) const
{
  auto memoryProperties = vk_physicalDevice.getMemoryProperties();
  for (std::size_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    if ((filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & vk_memoryProperties) == vk_memoryProperties)
      return i;
  }

  throw std::runtime_error("error @ pecs::Renderer::findMemoryIndex() : failed to find suitable memory index");
}

void Renderer::createCommandPools(const vk::raii::Device& vk_device, const std::vector<unsigned int>& indices)
{
  vk::CommandPoolCreateInfo ci_renderPool{
    .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = indices[0]
  };
  vk_renderPool = vk_device.createCommandPool(ci_renderPool);
  
  vk::CommandPoolCreateInfo ci_transferPool{
    .flags            = vk::CommandPoolCreateFlagBits::eTransient,
    .queueFamilyIndex = indices[3]
  };
  vk_transferPool = vk_device.createCommandPool(ci_transferPool);
}

void Renderer::createRenderBuffers(const vk::raii::Device& vk_device)
{  
  vk::CommandBufferAllocateInfo i_renderBuffers{
    .commandPool        = *vk_renderPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = settings.maxFlightFrames
  };

  vk_renderBuffers = vk_device.allocateCommandBuffers(i_renderBuffers);
}

void Renderer::createObjectBuffers(std::vector<Object *>& objects, const Device& device)
{    
  vk::raii::DeviceMemory vk_stagingMemory = nullptr;
  std::vector<vk::raii::Buffer> vk_stagingBuffers;
  
  std::size_t objectIndex = 0;
  vk::DeviceSize previousObjectSize = 0;
  std::array<vk::DeviceSize, 2> allocationSizes = { 0, 0 };
  std::array<vk::DeviceSize, 2> previousOffsets = { 0, 0 };
  std::vector<vk::DeviceSize> stagingOffsets;
  std::vector<vk::DeviceSize> vertexOffsets;

  for (auto * object : objects)
  {
    vk::DeviceSize objectSize = sizeof(object->vertices[0]) * object->vertices.size();

    vk::BufferCreateInfo ci_stagingBuffer{
      .size         = objectSize,
      .usage        = vk::BufferUsageFlagBits::eTransferSrc,
      .sharingMode  = vk::SharingMode::eExclusive
    };
    vk_stagingBuffers.emplace_back(device.logical().createBuffer(ci_stagingBuffer));

    vk::DeviceSize alignmentShift = 0;
    vk::DeviceSize offset = previousOffsets[0] + previousObjectSize;
    while (offset % vk_stagingBuffers[objectIndex].getMemoryRequirements().alignment != 0)
    {
      ++offset;
      ++alignmentShift;
    }

    allocationSizes[0] += vk_stagingBuffers[objectIndex].getMemoryRequirements().size + alignmentShift;
    previousOffsets[0] = offset;
    stagingOffsets.emplace_back(offset);
    
     vk::BufferCreateInfo ci_vertexBuffer{
      .size         = objectSize,
      .usage        = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
      .sharingMode  = vk::SharingMode::eExclusive
    };
    vk_vertexBuffers.emplace_back(device.logical().createBuffer(ci_vertexBuffer));
    
    offset = previousOffsets[1] + previousObjectSize;
    while (offset % vk_vertexBuffers[objectIndex].getMemoryRequirements().alignment != 0)
    {
      ++offset;
      ++alignmentShift;
    }

    allocationSizes[1] += vk_vertexBuffers[objectIndex].getMemoryRequirements().size + alignmentShift;
    previousOffsets[1] = offset;
    vertexOffsets.emplace_back(offset);

    previousObjectSize = objectSize;
    ++objectIndex;
  }

  vk::MemoryAllocateInfo i_stagingAllocate{
    .allocationSize   = allocationSizes[0],
    .memoryTypeIndex  = findMemoryIndex(device.physical(), vk_stagingBuffers[0].getMemoryRequirements().memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
  };
  vk_stagingMemory = device.logical().allocateMemory(i_stagingAllocate);
  
  vk::MemoryAllocateInfo i_vertexAllocate{
    .allocationSize = allocationSizes[1],
    .memoryTypeIndex = findMemoryIndex(device.physical(), vk_vertexBuffers[0].getMemoryRequirements().memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal)
  };
  vk_vertexMemory = device.logical().allocateMemory(i_vertexAllocate);

  objectIndex = 0;
  for (auto * object : objects)
  {
    vk::DeviceSize objectSize = sizeof(object->vertices[0]) * object->vertices.size();
    
    vk_stagingBuffers[objectIndex].bindMemory(*vk_stagingMemory, stagingOffsets[objectIndex]);
    vk_vertexBuffers[objectIndex].bindMemory(*vk_vertexMemory, vertexOffsets[objectIndex]);

    void * vertexData = vk_stagingMemory.mapMemory(stagingOffsets[objectIndex], objectSize);
    memcpy(vertexData, object->vertices.data(), objectSize);
    vk_stagingMemory.unmapMemory();

    ++objectIndex;
  }

  vk::FenceCreateInfo ci_transferFence;
  auto vk_transferFence = device.logical().createFence(ci_transferFence);

   vk::CommandBufferAllocateInfo ai_transferBuffer{
    .commandPool        = *vk_transferPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = static_cast<unsigned int>(objects.size())
  };
  vk_transferBuffers = device.logical().allocateCommandBuffers(ai_transferBuffer);

  for (std::size_t i = 0; i < vk_stagingBuffers.size(); ++i)
  {
    vk::CommandBufferBeginInfo bi_transferBuffer{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };
    vk_transferBuffers[i].begin(bi_transferBuffer);

    vk::BufferCopy bufferCopy{
      .srcOffset  = 0,
      .dstOffset  = 0,
      .size       = sizeof(objects[i]->vertices[0]) * objects[i]->vertices.size()
    };
    vk_transferBuffers[i].copyBuffer(*vk_stagingBuffers[i], *vk_vertexBuffers[i], bufferCopy);

    vk_transferBuffers[i].end();
  }

  std::vector<vk::CommandBuffer> transferBuffers;
  for (auto& vk_transferBuffer : vk_transferBuffers)
    transferBuffers.emplace_back(*vk_transferBuffer);
  
  vk::SubmitInfo i_transferSubmit{
    .commandBufferCount = static_cast<unsigned int>(transferBuffers.size()),
    .pCommandBuffers    = transferBuffers.data()
  };
  device.queue(QueueType::Transfer).submit(i_transferSubmit, *vk_transferFence);

  static_cast<void>(device.logical().waitForFences({ *vk_transferFence }, vk::True, UINT64_MAX));
  vk_transferBuffers.clear();
}

void Renderer::beginRendering(const unsigned int& frameIndex, const vk::raii::Image& vk_image, const vk::raii::ImageView& vk_imageView)
{ 
  vk::CommandBufferBeginInfo bi_commandBuffer;
  vk_renderBuffers[frameIndex].begin(bi_commandBuffer);

  vk::ImageMemoryBarrier memoryBarrier{
    .dstAccessMask    = vk::AccessFlagBits::eColorAttachmentWrite,
    .oldLayout        = vk::ImageLayout::eUndefined,
    .newLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    .image            = *vk_image,
    .subresourceRange = {
      .aspectMask       = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel     = 0,
      .levelCount       = 1,
      .baseArrayLayer   = 0,
      .layerCount       = 1
    }
  };

  vk_renderBuffers[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eTopOfPipe,
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    memoryBarrier
  );

  vk::RenderingAttachmentInfo i_attachment{
    .imageView    = *vk_imageView,
    .imageLayout  = vk::ImageLayout::eColorAttachmentOptimal,
    .loadOp       = vk::AttachmentLoadOp::eClear,
    .storeOp      = vk::AttachmentStoreOp::eStore,
    .clearValue   = vk_clearValue
  };

  vk::RenderingInfo i_rendering{
    .renderArea           = renderArea,
    .layerCount           = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &i_attachment
  };

  vk_renderBuffers[frameIndex].beginRenderingKHR(i_rendering);
}

void Renderer::endRendering(const unsigned int& frameIndex, const vk::raii::Image& vk_image)
{
  vk_renderBuffers[frameIndex].endRendering();
  
  vk::ImageMemoryBarrier memoryBarrier{
    .srcAccessMask    = vk::AccessFlagBits::eColorAttachmentWrite,
    .oldLayout        = vk::ImageLayout::eColorAttachmentOptimal,
    .newLayout        = vk::ImageLayout::ePresentSrcKHR,
    .image            = *vk_image,
    .subresourceRange = {
      .aspectMask       = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel     = 0,
      .levelCount       = 1,
      .baseArrayLayer   = 0,
      .layerCount       = 1
    }
  };
  
  vk_renderBuffers[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    memoryBarrier
  );

  vk_renderBuffers[frameIndex].end();
}

} // namespace pecs