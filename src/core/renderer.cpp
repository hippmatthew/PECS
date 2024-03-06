/*
 *  PECS::core - renderer.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/4/24
 *  Updated:  3/5/24
 */

#include "src/core/include/renderer.hpp"

namespace pecs
{

Renderer::Renderer(const Settings::Renderer& s, const Device& device, const ViewportInfo& vi) : settings(s), i_viewport(vi)
{
  createCommandPools(device.logical(), device.queueFamilyArray());
  createRenderBuffers(device.logical());
}

const std::vector<vk::raii::CommandBuffer>& Renderer::commandBuffers() const
{
  return vk_renderBuffers;
}

unsigned int Renderer::maxFlightFrames() const
{
  return settings.maxFlightFrames;
}

void Renderer::render(const std::vector<Object *>& objects, const unsigned int& frameIndex, const vk::Image& vk_image, const vk::raii::ImageView& vk_imageView, const Device& device)
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

  for (const auto * object : objects)
  {     
    vk_renderBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *(object->graphics->vk_pipeline));
    
    std::vector<vk::DescriptorSet> descriptorSets;
    for (auto& vk_descriptorSet : object->graphics->vk_descriptorSets[frameIndex])
      descriptorSets.emplace_back(*vk_descriptorSet);
    
    vk_renderBuffers[frameIndex].bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      *(object->graphics->vk_pipelineLayout),
      0,
      descriptorSets,
      nullptr);
    
    vk_renderBuffers[frameIndex].pushConstants<glm::mat4>(
      *(object->graphics->vk_pipelineLayout),
      vk::ShaderStageFlagBits::eVertex,
      0,
      object->graphics->model
    );
    
    vk_renderBuffers[frameIndex].bindVertexBuffers(0, *(object->graphics->vk_vertexBuffer), { 0 } );
    vk_renderBuffers[frameIndex].bindIndexBuffer(*(object->graphics->vk_indexBuffer), 0, vk::IndexType::eUint32);

    vk_renderBuffers[frameIndex].drawIndexed(object->graphics->indices.size(), 1, 0, 0, 0);
  }

  endRendering(frameIndex, vk_image);
}

void Renderer::createCommandPools(const vk::raii::Device& vk_device, const std::vector<unsigned int>& indices)
{
  vk::CommandPoolCreateInfo ci_renderPool{
    .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = indices[0]
  };
  vk_renderPool = vk_device.createCommandPool(ci_renderPool);
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

void Renderer::beginRendering(const unsigned int& frameIndex, const vk::Image& vk_image, const vk::raii::ImageView& vk_imageView)
{ 
  vk::CommandBufferBeginInfo bi_commandBuffer;
  vk_renderBuffers[frameIndex].begin(bi_commandBuffer);

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
    .clearValue   = settings.backgroundColor
  };

  vk::RenderingInfo i_rendering{
    .renderArea           = { .offset = { 0, 0 },
                              .extent = i_viewport.first },
    .layerCount           = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments    = &i_attachment
  };

  vk_renderBuffers[frameIndex].beginRenderingKHR(i_rendering);
}

void Renderer::endRendering(const unsigned int& frameIndex, const vk::Image& vk_image)
{
  vk_renderBuffers[frameIndex].endRendering();
  
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