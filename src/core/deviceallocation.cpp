/*
 *  PECS::core - deviceallocation.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/17/24
 *  Updated:  2/20/24
 */

#include "src/core/include/deviceallocation.hpp"

namespace pecs
{

DeviceAllocation::DeviceAllocation(const Device * d, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
: device(d),
  vertexSize(sizeof(vertices[0]) * vertices.size()), vertexData(vertices.data()),
  indexSize(sizeof(indices[0]) * indices.size()), indexData(indices.data())
{
  createBuffers();
}

DeviceAllocation::~DeviceAllocation()
{
  vk_projectionMemory.unmapMemory();
  vk_propertiesMemory.unmapMemory();

  for (auto& vk_otherMemory : vk_otherMemories)
    vk_otherMemory.unmapMemory();

  device = nullptr;
  vertexData = nullptr;
  indexData = nullptr;
}

void DeviceAllocation::allocate(std::size_t maxFlightFrames)
{
  vk::BufferCreateInfo ci_vertexStagingBuffer{
    .size         = vertexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  auto vk_vertexStagingBuffer = device->logical().createBuffer(ci_vertexStagingBuffer);

  vk::BufferCreateInfo ci_indexStagingBuffer{
    .size         = indexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferSrc,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  auto vk_indexStagingBuffer = device->logical().createBuffer(ci_indexStagingBuffer);

  vk::DeviceSize offset = vk_vertexStagingBuffer.getMemoryRequirements().size;
  while (offset % vk_indexStagingBuffer.getMemoryRequirements().alignment != 0)
    ++offset;

  vk::MemoryAllocateInfo i_stagingMemory{
    .allocationSize   = vk_indexStagingBuffer.getMemoryRequirements().size + offset,
    .memoryTypeIndex  = findMemoryIndex(
                          vk_vertexStagingBuffer.getMemoryRequirements().memoryTypeBits |
                            vk_indexStagingBuffer.getMemoryRequirements().memoryTypeBits,
                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                        )
  };
  auto vk_stagingMemory = device->logical().allocateMemory(i_stagingMemory);

  vk_vertexStagingBuffer.bindMemory(*vk_stagingMemory, 0);
  vk_indexStagingBuffer.bindMemory(*vk_stagingMemory, offset);

  void * vData = vk_stagingMemory.mapMemory(0, vertexSize);
  memcpy(vData, vertexData, vertexSize);
  vk_stagingMemory.unmapMemory();

  void * iData = vk_stagingMemory.mapMemory(offset, indexSize);
  memcpy(iData, indexData, indexSize);
  vk_stagingMemory.unmapMemory();
  
  offset = vk_vertexBuffer.getMemoryRequirements().size;
  while (offset % vk_indexBuffer.getMemoryRequirements().alignment != 0)
    ++offset;

  vk::MemoryAllocateInfo i_modelMemory{
    .allocationSize   = vk_indexBuffer.getMemoryRequirements().size + offset,
    .memoryTypeIndex  = findMemoryIndex(
                          vk_vertexBuffer.getMemoryRequirements().memoryTypeBits |
                            vk_indexBuffer.getMemoryRequirements().memoryTypeBits,
                          vk::MemoryPropertyFlagBits::eDeviceLocal
                        )
  };
  vk_modelMemory = device->logical().allocateMemory(i_modelMemory);

  vk_vertexBuffer.bindMemory(*vk_modelMemory, 0);
  vk_indexBuffer.bindMemory(*vk_modelMemory, offset);

  vk::CommandPoolCreateInfo ci_transferPool{
    .flags            = vk::CommandPoolCreateFlagBits::eTransient,
    .queueFamilyIndex = device->queueFamilyArray()[3]
  };
  auto vk_transferPool = device->logical().createCommandPool(ci_transferPool);

  vk::CommandBufferAllocateInfo i_transferBuffers{
    .commandPool        = *vk_transferPool,
    .level              = vk::CommandBufferLevel::ePrimary,
    .commandBufferCount = 2
  };
  auto vk_transferBuffers = device->logical().allocateCommandBuffers(i_transferBuffers);

  vk::CommandBufferBeginInfo bi_transferBuffer{
    .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };
  
  vk_transferBuffers[0].begin(bi_transferBuffer);
  vk_transferBuffers[1].begin(bi_transferBuffer);

  vk::BufferCopy vertexCopy{
    .srcOffset  = 0,
    .dstOffset  = 0,
    .size       = vertexSize
  };
  vk_transferBuffers[0].copyBuffer(*vk_vertexStagingBuffer, *vk_vertexBuffer, vertexCopy);

  vk::BufferCopy indexCopy{
    .srcOffset  = 0,
    .dstOffset  = 0,
    .size       = indexSize
  };
  vk_transferBuffers[1].copyBuffer(*vk_indexStagingBuffer, *vk_indexBuffer, indexCopy);

  vk_transferBuffers[1].end();
  vk_transferBuffers[0].end();

  vk::FenceCreateInfo ci_transferFence;
  auto vk_transferFence = device->logical().createFence(ci_transferFence);

  std::array<vk::CommandBuffer, 2> transferBuffers = { *(vk_transferBuffers[0]), *(vk_transferBuffers[1]) };

  vk::SubmitInfo i_transferSubmit{
    .commandBufferCount = 2,
    .pCommandBuffers    = transferBuffers.data()
  };
  device->queue(QueueType::Transfer).submit(i_transferSubmit, *vk_transferFence);

  static_cast<void>(device->logical().waitForFences({ *vk_transferFence }, vk::True, UINT64_MAX));

  vk::MemoryAllocateInfo i_projectionMemory{
    .allocationSize   = sizeof(ProjectionData),
    .memoryTypeIndex  = findMemoryIndex(vk_projectionBuffer.getMemoryRequirements().memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
  };
  vk_projectionMemory = device->logical().allocateMemory(i_projectionMemory);

  vk_projectionBuffer.bindMemory(*vk_projectionMemory, 0);
  projectionMapping = vk_projectionMemory.mapMemory(0, sizeof(ProjectionData));

  vk::MemoryAllocateInfo i_propertiesMemory{
    .allocationSize   = sizeof(PhysicalProperties),
    .memoryTypeIndex  = findMemoryIndex(vk_propertiesBuffer.getMemoryRequirements().memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
  };
  vk_propertiesMemory = device->logical().allocateMemory(i_propertiesMemory);

  vk_propertiesBuffer.bindMemory(*vk_propertiesMemory, 0);
  propertiesMapping = vk_propertiesMemory.mapMemory(0, sizeof(PhysicalProperties));

  unsigned long bufferIndex = 0;
  for (const auto& vk_otherBuffer : vk_otherBuffers)
  {
    vk::MemoryAllocateInfo i_otherMemory{
      .allocationSize   = objectSizes[objectTypes[bufferIndex]],
      .memoryTypeIndex  = findMemoryIndex(vk_otherBuffer.getMemoryRequirements().memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
    };
    vk_otherMemories.emplace_back(device->logical().allocateMemory(i_otherMemory));

    vk_otherBuffer.bindMemory(*(vk_otherMemories[bufferIndex]), 0);

    otherMappings.emplace_back(vk_otherMemories[bufferIndex].mapMemory(0, objectSizes[objectTypes[bufferIndex]]));
  }
}

void DeviceAllocation::createDescriptors(std::size_t maxFlightFrames, const std::vector<vk::raii::DescriptorSetLayout>& vk_descriptorLayouts)
{
  vk::DescriptorPoolSize poolSize{
    .type             = vk::DescriptorType::eUniformBuffer,
    .descriptorCount  = static_cast<unsigned int>(maxFlightFrames * (2 + vk_otherBuffers.size()))
  };
  
  vk::DescriptorPoolCreateInfo ci_descriptorPool{
    .flags          = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    .maxSets        = static_cast<unsigned int>(maxFlightFrames * (2 + vk_otherBuffers.size())),
    .poolSizeCount  = 1,
    .pPoolSizes     = &poolSize
  };

  vk_descriptorPool = device->logical().createDescriptorPool(ci_descriptorPool);

  auto projectionLayout = *vk_descriptorLayouts[0];
  auto propertiesLayout = *vk_descriptorLayouts[1];
  
  std::optional<vk::DescriptorSetLayout> otherLayout;
  if (vk_descriptorLayouts.size() == 3)
    otherLayout = *vk_descriptorLayouts[2];
  
  for (unsigned long i = 0; i < maxFlightFrames; ++i)
  {    
    vk::DescriptorSetAllocateInfo i_projectionAllocate{
      .descriptorPool     = *vk_descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts        = &projectionLayout
    };
    vk_projectionSets.emplace_back(device->logical().allocateDescriptorSets(i_projectionAllocate));

    vk::DescriptorSetAllocateInfo i_propertiesAllocate{
      .descriptorPool     = *vk_descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts        = &propertiesLayout
    };
    vk_propertiesSets.emplace_back(device->logical().allocateDescriptorSets(i_propertiesAllocate));
    
    vk::DescriptorBufferInfo i_projectionDescriptor{
      .buffer = *vk_projectionBuffer,
      .offset = 0,
      .range  = sizeof(ProjectionData)
    };
    
    vk::WriteDescriptorSet projectionWrite{
      .dstSet           = *vk_projectionSets[i][0],
      .dstBinding       = 0,
      .dstArrayElement  = 0,
      .descriptorCount  = 1,
      .descriptorType   = vk::DescriptorType::eUniformBuffer,
      .pBufferInfo      = &i_projectionDescriptor
    };

    vk::DescriptorBufferInfo i_propertiesDescriptor{
      .buffer = *vk_propertiesBuffer,
      .offset = 0,
      .range  = sizeof(PhysicalProperties)
    };

    vk::WriteDescriptorSet propertiesWrite{
      .dstSet           = *vk_propertiesSets[i][0],
      .dstBinding       = 0,
      .dstArrayElement  = 0,
      .descriptorCount  = 1,
      .descriptorType   = vk::DescriptorType::eUniformBuffer,
      .pBufferInfo      = &i_propertiesDescriptor
    };

    std::vector<vk::WriteDescriptorSet> objectWrites = { projectionWrite, propertiesWrite };
    device->logical().updateDescriptorSets(objectWrites, nullptr);
    
    if (!otherLayout.has_value()) continue;
    
    vk::DescriptorSetAllocateInfo i_otherAllocate{
      .descriptorPool     = *vk_descriptorPool,
      .descriptorSetCount = static_cast<unsigned int>(vk_otherBuffers.size()),
      .pSetLayouts        = &(otherLayout.value())
    };
    vk_otherSets.emplace_back(device->logical().allocateDescriptorSets(i_otherAllocate));

    unsigned long bufferIndex = 0;
    std::vector<vk::WriteDescriptorSet> otherWrites;
    for (auto& vk_otherBuffer : vk_otherBuffers)
    {
      vk::DescriptorBufferInfo i_otherDescriptor{
        .buffer = *vk_otherBuffer,
        .offset = 0,
        .range  = objectSizes[objectTypes[bufferIndex]]
      };

      vk::WriteDescriptorSet otherWrite{
        .dstSet           = *vk_otherSets[i][bufferIndex],
        .dstBinding       = static_cast<unsigned int>(bufferIndex),
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo      = &i_otherDescriptor
      };

      otherWrites.emplace_back(otherWrite);
      ++bufferIndex;
    }

    device->logical().updateDescriptorSets(otherWrites, nullptr);
  }
}

template <typename T>
unsigned long DeviceAllocation::addAllocation(T& object, std::string objectType)
{
  bool existingType = false;
  for (auto& tag : objectTypes)
  {
    if (tag == objectType)
    {
      existingType = true;
      break;
    }
  }
  if (!existingType)
    objectSizes.emplace(std::pair<std::string, unsigned long>(objectType, sizeof(T)));

  objectTypes.emplace_back(objectType);

  vk::BufferCreateInfo ci_buffer{
    .size         = sizeof(T),
    .usage        = vk::BufferUsageFlagBits::eUniformBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  
  vk_otherBuffers.emplace_back(device->logical().createBuffer(ci_buffer));
}

void DeviceAllocation::updateProjection(ProjectionData& projection)
{
  memcpy(projectionMapping, &projection, sizeof(projection));
}

void DeviceAllocation::updateProperties(PhysicalProperties& properties)
{
  memcpy(propertiesMapping, &properties, sizeof(properties));
}

template <typename T>
void DeviceAllocation::updateBuffer(unsigned long index, T& object)
{
  memcpy(otherMappings[index], &object, sizeof(object));
}

unsigned int DeviceAllocation::findMemoryIndex(unsigned int filter, vk::MemoryPropertyFlags vk_memoryProperties)
{
  auto memoryProperties = device->physical().getMemoryProperties();
  for (unsigned long i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    if ((filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & vk_memoryProperties) == vk_memoryProperties)
      return i;
  }

  throw std::runtime_error("error @ pecs::DeviceAllocation::findMemoryIndex() : failed to find suitable memory index");
}

void DeviceAllocation::createBuffers()
{
  vk::BufferCreateInfo ci_vertexBuffer{
    .size         = vertexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_vertexBuffer = device->logical().createBuffer(ci_vertexBuffer);

  vk::BufferCreateInfo ci_indexBuffer{
    .size         = indexSize,
    .usage        = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_indexBuffer = device->logical().createBuffer(ci_indexBuffer);

  vk::BufferCreateInfo ci_projectionBuffer{
    .size         = sizeof(ProjectionData),
    .usage        = vk::BufferUsageFlagBits::eUniformBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_projectionBuffer = device->logical().createBuffer(ci_projectionBuffer);

  vk::BufferCreateInfo ci_propertiesBuffer{
    .size         = sizeof(PhysicalProperties),
    .usage        = vk::BufferUsageFlagBits::eUniformBuffer,
    .sharingMode  = vk::SharingMode::eExclusive
  };
  vk_propertiesBuffer = device->logical().createBuffer(ci_propertiesBuffer);
}

} // namespace pecs