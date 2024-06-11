#include "src/core/include/device.hpp"

namespace pecs
{

bool Device::QueueFamily::operator & (unsigned int bits) const
{
  return qf_types & bits;
}

Device::QueueFamily& Device::QueueFamily::operator ^ (unsigned int bits)
{
  qf_types ^= bits;
  return *this;
}

const vk::raii::Queue& Device::QueueFamily::queue() const
{
  return qf_queue;
}

} // namespace pecs