#include "src/core/include/device.hpp"

namespace pecs
{

Device::QueueFamily::QueueFamily(unsigned long queueIndex, unsigned int queueTypes)
: qf_index(queueIndex), qf_types(queueTypes)
{}

Device::QueueFamilies::QueueFamilies(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::SurfaceKHR& vk_surface)
{
  std::queue<unsigned long> allQueueIndices, asyncComputeIndices, asyncTransferIndices, asyncCombineIndices, sparseIndices;

  unsigned long index = 0;
  for (const auto& queueFamily : vk_physicalDevice.getQueueFamilyProperties())
  {
    unsigned int queueTypes = 0x0000000u;
    
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics && vk_physicalDevice.getSurfaceSupportKHR(index, *vk_surface))
      queueTypes |= PECS_GRAPHICS_QUEUE_BIT | PECS_PRESENT_QUEUE_BIT;

    if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute && queueTypes & PECS_GRAPHICS_QUEUE_BIT)
      queueTypes |= PECS_SYNC_COMPUTE_BIT;
    else if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute && !(queueTypes & PECS_GRAPHICS_QUEUE_BIT))
      queueTypes |= PECS_ASYNC_COMPUTE_BIT;

    if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer && queueTypes & PECS_GRAPHICS_QUEUE_BIT)
      queueTypes |= PECS_SYNC_TRANSFER_BIT;
    else if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer && !(queueTypes & PECS_GRAPHICS_QUEUE_BIT))
      queueTypes |= PECS_ASYNC_TRANSFER_BIT;

    if (queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding)
      queueTypes |= PECS_SPARSE_BIT;

    switch (to_family(queueTypes))
    {
      case FamilyType::AllQueue:
        allQueueIndices.emplace(index);
        break;
      case FamilyType::AsyncCombine:
        asyncCombineIndices.emplace(index);
        break;
      case FamilyType::Compute:
        asyncComputeIndices.emplace(index);
        break;
      case FamilyType::Transfer:
        asyncTransferIndices.emplace(index);
        break;
      case FamilyType::Sparse:
        sparseIndices.emplace(index);
      default:
        break;
    }
  }

  if (!allQueueIndices.empty())
  {
    addFamily(FamilyType::AllQueue, allQueueIndices.front());
    allQueueIndices.pop();
  }
      
  if (!asyncComputeIndices.empty() && !asyncTransferIndices.empty())
  {
    addFamily(FamilyType::Compute, asyncCombineIndices.front());
    addFamily(FamilyType::Transfer, asyncTransferIndices.front());
  }
  else if (asyncCombineIndices.size() >= 2)
  {
    addFamily(FamilyType::Compute, asyncCombineIndices.front());
    asyncCombineIndices.pop();
    addFamily(FamilyType::Transfer, asyncCombineIndices.front());
  }
  else if (allQueueIndices.size() >= 2)
  {
    addFamily(FamilyType::Compute, allQueueIndices.front());
    allQueueIndices.pop();
    addFamily(FamilyType::Transfer, allQueueIndices.front());
  }
  else if (!asyncCombineIndices.empty())
    addFamily(FamilyType::AsyncCombine, asyncCombineIndices.front());
  else if (!allQueueIndices.empty())
    addFamily(FamilyType::AsyncCombine, allQueueIndices.front());
  
  if (!sparseIndices.empty())
    addFamily(FamilyType::Sparse, sparseIndices.front());
}

Device::QueueFamilies::~QueueFamilies()
{
  for (const auto& familyType : familyTypes)
    delete familyMap.at(std::to_string(familyType));
}

void Device::QueueFamilies::setQueues(const vk::raii::Device& vk_device)
{
  for (const auto& familyType : familyTypes)
  {
    auto * family = familyMap.at(std::to_string(familyType));
    family->qf_queue = vk_device.getQueue(family->qf_index, 0);
  }
}

void Device::QueueFamilies::addFamily(FamilyType type, unsigned long index)
{
  unsigned int queueTypes = 0x0000000u;

  switch (type)
  {
    case AllQueue:
      queueTypes = PECS_ALLQUEUE_FAMILY;
      break;
    case Compute:
      queueTypes = PECS_ASYNC_COMPUTE_FAMILY;
      break;
    case Transfer:
      queueTypes = PECS_ASYNC_TRANSFER_FAMILY;
      break;
    case AsyncCombine:
      queueTypes = PECS_ASYNC_COMBINE_FAMILY;
      break;
    case Sparse:
      queueTypes = PECS_SPARSE_FAMILY;
      break;
    default:
      return;
  }

  familyMap.emplace(std::pair<std::string, QueueFamily *>(std::to_string(type), new QueueFamily(index, queueTypes)));
  familyTypes.emplace_back(type);
}

} // namespace pecs