#include "src/core/include/synchronization.hpp"

namespace vecs
{

Synchronization * Synchronization::p_sync = nullptr;

Synchronization& Synchronization::instance()
{
  if (p_sync == nullptr)
    p_sync = new Synchronization;

  return *p_sync;
}

void Synchronization::destroy()
{  
  if (p_sync == nullptr) return;

  p_sync->fenceMap.clear();
  p_sync->semaphoreMap.clear();
  p_sync->p_device.reset();

  delete p_sync;
  p_sync = nullptr;
}

const vk::raii::Fence& Synchronization::fence(std::string tag) const
{
  return fenceMap.at(tag);
}

const vk::raii::Semaphore& Synchronization::semaphore(std::string tag) const
{
  return semaphoreMap.at(tag);
}

void Synchronization::wait_fences(std::vector<std::string> tags) const
{
  std::vector<vk::Fence> fences;
  for (const auto& tag : tags)
  {
    if (fenceMap.find(tag) == fenceMap.end()) continue;

    fences.emplace_back(*(fenceMap.at(tag)));
  }

  static_cast<void>(p_device->logical().waitForFences(
    vk::ArrayProxy<vk::Fence>(fences.size(), fences.data()),
    vk::True,
    UINT64_MAX
  ));
}

void Synchronization::add_fence(std::string tag, bool signaled)
{
  if (fenceMap.find(tag) != fenceMap.end()) return;
  
  vk::FenceCreateInfo ci_fence{
    .flags  = signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags()
  };

  fenceMap.emplace(std::make_pair(tag, p_device->logical().createFence(ci_fence)));
}

void Synchronization::add_semaphore(std::string tag)
{
  if (semaphoreMap.find(tag) != semaphoreMap.end()) return;
  
  vk::SemaphoreCreateInfo ci_semaphore;
  semaphoreMap.emplace(std::make_pair(tag, p_device->logical().createSemaphore(ci_semaphore)));
}

void Synchronization::remove_fence(std::string tag)
{
  if (fenceMap.find(tag) == fenceMap.end()) return;

  fenceMap.erase(tag);
}

void Synchronization::remove_semaphore(std::string tag)
{
  if (semaphoreMap.find(tag) != semaphoreMap.end()) return;

  semaphoreMap.erase(tag);
}

void Synchronization::link_device(std::shared_ptr<Device> link)
{
  if (p_device != nullptr) return;

  p_device = link;
}

}