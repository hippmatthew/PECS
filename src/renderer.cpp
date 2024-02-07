/*
 *  PECS - renderer.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/4/24
 *  Updated:  2/6/24
 */

#include "src/include/renderer.hpp"

namespace pecs
{

Renderer::Renderer(const Device& device, const ViewportInfo& vi)
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

void Renderer::render(const Object& object, const vk::raii::Image& vk_image, const vk::raii::ImageView& vk_imageView)
{  
  vk::CommandBufferBeginInfo bi_commandBuffer;
  vk_commandBuffers[0].begin(bi_commandBuffer);

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
  vk_commandBuffers[0].pipelineBarrier(
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

  vk_commandBuffers[0].beginRenderingKHR(i_rendering);

  vk_commandBuffers[0].bindPipeline(vk::PipelineBindPoint::eGraphics, *object.graphicsPipeline());

  vk::Viewport vk_viewport{
    .x = 0.0f,
    .y = 0.0f,
    .width = static_cast<float>(i_viewport.first.width),
    .height = static_cast<float>(i_viewport.first.height),
    .minDepth = 0.0f,
    .maxDepth = 0.0f
  };
  vk_commandBuffers[0].setViewport(0, vk_viewport);

  vk::Rect2D vk_scissor{
    .offset = {0, 0},
    .extent = i_viewport.first
  };
  vk_commandBuffers[0].setScissor(0, vk_scissor);

  vk_commandBuffers[0].draw(object.vertices, 1, 0, 0);

  vk_commandBuffers[0].endRendering();

  memoryBarrier.dstAccessMask = {};
  memoryBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
  memoryBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
  
  vk_commandBuffers[0].pipelineBarrier(
    vk::PipelineStageFlagBits::eColorAttachmentOutput,
    vk::PipelineStageFlagBits::eBottomOfPipe,
    vk::DependencyFlags(),
    nullptr,
    nullptr,
    memoryBarrier
  );

  vk_commandBuffers[0].end();
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
    .commandBufferCount = 1
  };

  vk_commandBuffers = vk_device.allocateCommandBuffers(i_allocate);
}

} // namespace pecs