/*
 *  PECS - queues.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#include "src/include/device.hpp"

namespace pecs
{

Device::Queues::Queues(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::SurfaceKHR& vk_surface)
{
  int index = 0;
  for (const auto& queueFamily : vk_physicalDevice.getQueueFamilyProperties())
  {
    if (!graphics.first.has_value() && !present.first.has_value() && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
    { 
      graphics.first = index++;
      continue;
    }

    if (!present.first.has_value() && vk_physicalDevice.getSurfaceSupportKHR(index, *vk_surface))
    {
      present.first = index++;
      continue;
    }

    if (!compute.first.has_value() && queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
    {
      compute.first = index++;
      continue;
    }

    if (!transfer.first.has_value() && queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
    {
      transfer.first = index++;
      continue;
    }

    if (isComplete()) break;
  }
}

unsigned int Device::Queues::index(QueueType type) const
{
  switch(type)
  {
    case Graphics:
      return graphics.first.value();
    case Present:
      return present.first.value();
    case Compute:
      return compute.first.value();
    case Transfer:
      return transfer.first.value();
  }
}

const vk::raii::Queue& Device::Queues::queue(QueueType type) const
{
  switch(type)
  {
    case Graphics:
      return graphics.second;
    case Present:
      return present.second;
    case Compute:
      return compute.second;
    case Transfer:
      return transfer.second;
  }
}

std::vector<unsigned int> Device::Queues::array() const
{
  return std::vector<unsigned int>{graphics.first.value(), present.first.value(), compute.first.value(), transfer.first.value()};
}

bool Device::Queues::isComplete() const
{
  return graphics.first.has_value() && present.first.has_value() && compute.first.has_value() && transfer.first.has_value();
}

void Device::Queues::setQueues(const vk::raii::Device& vk_device)
{
  graphics.second = vk_device.getQueue(graphics.first.value(), 0);
  present.second = vk_device.getQueue(present.first.value(), 0);
  compute.second = vk_device.getQueue(compute.first.value(), 0);
  transfer.second = vk_device.getQueue(transfer.first.value(), 0);
}

} // namespace pecs