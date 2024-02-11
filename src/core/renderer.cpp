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

  vk::DeviceSize vertexOffset = 0, indexOffset = 0;
  for (auto * object : objects)
  {
    vk::DeviceSize vertexSize = sizeof(object->vertices[0]) * object->vertices.size();
    vk::DeviceSize indexSize = sizeof(object->indices[0]) * object->indices.size();
    
    vk_renderBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *object->vk_graphicsPipeline);
    vk_renderBuffers[frameIndex].bindVertexBuffers(0, { *vk_vertexBuffer }, { vertexOffset });
    vk_renderBuffers[frameIndex].bindIndexBuffer(*vk_indexBuffer, indexOffset, vk::IndexType::eUint32);

    vk_renderBuffers[frameIndex].drawIndexed(object->indices.size(), 1, 0, 0, 0);
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
  vk::raii::Buffer vk_vertexStagingBuffer = nullptr;
  vk::raii::Buffer vk_indexStagingBuffer = nullptr;

  vk::DeviceSize totalVertexSize = 0, totalIndexSize = 0;
  for (auto * object : objects)
  {
    vk::DeviceSize vertexSize = sizeof(object->vertices[0]) * object->vertices.size();
    vk::DeviceSize indexSize = sizeof(object->indices[0]) * object->indices.size();

    totalVertexSize += vertexSize;
    totalIndexSize += indexSize;
  }

  vk::BufferCreateInfo ci_vertexStagingBuffer{
    .size         = totalVertexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_vertexStagingBuffer = device.logical().createBuffer(ci_vertexStagingBuffer);

  vk::BufferCreateInfo ci_indexStagingBuffer{
    .size         = totalIndexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_indexStagingBuffer = device.logical().createBuffer(ci_indexStagingBuffer);

  vk::DeviceSize offset = vk_vertexStagingBuffer.getMemoryRequirements().size;
  while (offset % vk_indexStagingBuffer.getMemoryRequirements().alignment != 0)
    ++offset;
  
  vk::MemoryAllocateInfo i_stagingMemory{
    .allocationSize   = vk_indexStagingBuffer.getMemoryRequirements().size + offset,
    .memoryTypeIndex  = findMemoryIndex(device.physical(),
                          vk_vertexStagingBuffer.getMemoryRequirements().memoryTypeBits |
                            vk_indexStagingBuffer.getMemoryRequirements().memoryTypeBits,
                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
  };
  vk_stagingMemory = device.logical().allocateMemory(i_stagingMemory);

  vk_vertexStagingBuffer.bindMemory(*vk_stagingMemory, 0);
  vk_indexStagingBuffer.bindMemory(*vk_stagingMemory, offset);

  vk::DeviceSize vertexOffset = 0, indexOffset = 0;
  for (auto * object : objects)
  {
    vk::DeviceSize vertexSize = sizeof(object->vertices[0]) * object->vertices.size();
    vk::DeviceSize indexSize = sizeof(object->indices[0]) * object->indices.size();

    void * data = vk_stagingMemory.mapMemory(vertexOffset, vertexSize);
    memcpy(data, object->vertices.data(), vertexSize);
    vk_stagingMemory.unmapMemory();

    data = vk_stagingMemory.mapMemory(offset + indexOffset, indexSize);
    memcpy(data, object->indices.data(), indexSize);
    vk_stagingMemory.unmapMemory();

    vertexOffset += vertexSize;
    indexOffset += indexSize;
  }

  vk::BufferCreateInfo ci_vertexBuffer{
    .size         = totalVertexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_vertexBuffer = device.logical().createBuffer(ci_vertexBuffer);

  vk::BufferCreateInfo ci_indexBuffer{
    .size         = totalIndexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_indexBuffer = device.logical().createBuffer(ci_indexBuffer);
  
  offset = vk_vertexBuffer.getMemoryRequirements().size;
  while (offset % vk_indexBuffer.getMemoryRequirements().alignment != 0)
    ++offset;
  
  vk::MemoryAllocateInfo i_objectMemory{
    .allocationSize   = vk_indexBuffer.getMemoryRequirements().size + offset,
    .memoryTypeIndex  = findMemoryIndex(device.physical(),
                          vk_vertexBuffer.getMemoryRequirements().memoryTypeBits |
                            vk_indexBuffer.getMemoryRequirements().memoryTypeBits,
                          vk::MemoryPropertyFlagBits::eDeviceLocal)
  };
  vk_objectMemory = device.logical().allocateMemory(i_objectMemory);

  vk_vertexBuffer.bindMemory(*vk_objectMemory, 0);
  vk_indexBuffer.bindMemory(*vk_objectMemory, offset);

  std::vector<vk::raii::CommandBuffer> vk_transferBuffers;
  vk::CommandBufferAllocateInfo i_transferBuffers{
    .commandPool        = *vk_transferPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = static_cast<unsigned int>(2 * objects.size())
  };
  vk_transferBuffers = device.logical().allocateCommandBuffers(i_transferBuffers);

  vertexOffset = indexOffset = 0;
  std::size_t bufferIndex = 0;
  for (auto * object : objects)
  {
    vk::DeviceSize vertexSize = sizeof(object->vertices[0]) * object->vertices.size();
    vk::DeviceSize indexSize = sizeof(object->indices[0]) * object->indices.size();

    vk::CommandBufferBeginInfo bi_transfer{
      .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    vk_transferBuffers[bufferIndex].begin(bi_transfer);
    vk_transferBuffers[bufferIndex + 1].begin(bi_transfer);

    vk::BufferCopy vertexCopy{
      .srcOffset  = vertexOffset,
      .dstOffset  = vertexOffset,
      .size       = vertexSize
    };
    vk_transferBuffers[bufferIndex].copyBuffer(*vk_vertexStagingBuffer, *vk_vertexBuffer, vertexCopy);

    vk::BufferCopy indexCopy{
      .srcOffset  = indexOffset,
      .dstOffset  = indexOffset,
      .size       = indexSize
    };
    vk_transferBuffers[bufferIndex + 1].copyBuffer(*vk_indexStagingBuffer, *vk_indexBuffer, indexCopy);

    vk_transferBuffers[bufferIndex + 1].end();
    vk_transferBuffers[bufferIndex].end();

    bufferIndex += 2;
    vertexOffset += vertexSize;
    indexOffset += indexSize;
  }

  vk::FenceCreateInfo ci_transferFence;
  vk::raii::Fence vk_transferFence = device.logical().createFence(ci_transferFence);

  std::vector<vk::CommandBuffer> transferBuffers;
  for (auto& vk_transferBuffer : vk_transferBuffers)
    transferBuffers.emplace_back(*vk_transferBuffer);

  vk::SubmitInfo i_transferSubmit{
    .commandBufferCount = static_cast<unsigned int>(transferBuffers.size()),
    .pCommandBuffers    = transferBuffers.data()
  };
  device.queue(QueueType::Transfer).submit(i_transferSubmit, { *vk_transferFence });

  static_cast<void>(device.logical().waitForFences({ *vk_transferFence }, vk::True, UINT64_MAX));
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