/*
 *  PECS - renderer.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/4/24
 *  Updated:  2/7/24
 */

#include "src/include/renderer.hpp"

#include <iostream>

namespace pecs
{

Renderer::Renderer(const Settings::Renderer& s, const Device& device, const ViewportInfo& vi) : settings(s)
{
  createCommandPool(device.logical(), device.queueFamilyArray());
  createCommandBuffers(device.logical());

  i_viewport = vi;
  renderArea.offset = {0, 0};
  renderArea.extent = i_viewport.first;
}

const std::vector<vk::raii::CommandBuffer>& Renderer::commandBuffers() const
{
  return vk_commandBuffers;
}

unsigned int Renderer::maxFlightFrames() const
{
  return settings.maxFlightFrames;
}

void Renderer::start(const unsigned int& frameIndex, const vk::raii::Image& vk_image, const vk::raii::ImageView& vk_imageView)
{
  vk_commandBuffers[frameIndex].reset();
  
  vk::CommandBufferBeginInfo bi_commandBuffer;
  vk_commandBuffers[frameIndex].begin(bi_commandBuffer);

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

  vk_commandBuffers[frameIndex].pipelineBarrier(
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

  vk_commandBuffers[frameIndex].beginRenderingKHR(i_rendering);
}

void Renderer::render(const Object& object, const unsigned int& frameIndex, const vk::raii::Image& vk_image, const vk::raii::ImageView& vk_imageView)
{  
  vk_commandBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *object.graphicsPipeline());

  vk::Viewport vk_viewport{
    .x = 0.0f,
    .y = 0.0f,
    .width = static_cast<float>(i_viewport.first.width),
    .height = static_cast<float>(i_viewport.first.height),
    .minDepth = 0.0f,
    .maxDepth = 0.0f
  };
  vk_commandBuffers[frameIndex].setViewport(0, vk_viewport);

  vk::Rect2D vk_scissor{
    .offset = {0, 0},
    .extent = i_viewport.first
  };
  vk_commandBuffers[frameIndex].setScissor(0, vk_scissor);

  vk_commandBuffers[frameIndex].draw(object.vertices, 1, 0, 0);
}

void Renderer::stop(const unsigned int& frameIndex, const vk::raii::Image& vk_image)
{
  vk_commandBuffers[frameIndex].endRendering();
  
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
  
  vk_commandBuffers[frameIndex].pipelineBarrier(
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    memoryBarrier
  );

  vk_commandBuffers[frameIndex].end();
}

void Renderer::createCommandPool(const vk::raii::Device& vk_device, const std::vector<unsigned int>& indices)
{
  vk::CommandPoolCreateInfo ci_commandPool{
    .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
    .queueFamilyIndex = indices[0]
  };

  vk_commandPool = vk_device.createCommandPool(ci_commandPool);
}

void Renderer::createCommandBuffers(const vk::raii::Device& vk_device)
{  
  vk::CommandBufferAllocateInfo i_allocate{
    .commandPool        = *vk_commandPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = settings.maxFlightFrames
  };

  vk_commandBuffers = vk_device.allocateCommandBuffers(i_allocate);
  std::cout << "command buffer size: " << vk_commandBuffers.size() << '\n';
}

} // namespace pecs