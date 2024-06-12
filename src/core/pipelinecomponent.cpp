#include "src/core/include/component.hpp"

namespace pecs
{

std::vector<char> PipelineComponent::readShader(std::string path) const
{
  std::vector<char> buffer;

  std::ifstream shader(path, std::ios::ate | std::ios::binary);
  if (shader.fail()) return buffer;

  std::size_t size = shader.tellg();
  buffer.resize(size);

  shader.seekg(0);
  shader.read(buffer.data(), size);

  shader.close();
  return buffer;
}

unsigned int PipelineComponent::findMemoryIndex(const vk::raii::PhysicalDevice& vk_physicalDevice, unsigned int filter, vk::MemoryPropertyFlags properties) const
{
  auto memoryProperties = vk_physicalDevice.getMemoryProperties();
  
  for (unsigned long i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    if ((filter & (1 << i)) &&
        (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
    {
      return i;
    }
  }
  
  throw std::runtime_error("error @ pecs::Engine::allocateCamera() : could not find suitable memory index");
}

} // namespace pecs