/*
 *  PECS::core - deviceallocation.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/17/24
 *  Updated:  2/20/24
 */

#ifndef pecs_core_deviceallocation_hpp
#define pecs_core_deviceallocation_hpp

#include "src/core/include/singular.hpp"
#include "src/core/include/device.hpp"

#ifndef pecs_include_glm
#define pecs_include_glm

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#endif // pecs_include_glm

#include <map>

namespace pecs
{

struct Vertex
{
  glm::vec3 position;

  static vk::VertexInputBindingDescription bindingDescription()
  {
    return vk::VertexInputBindingDescription{
      .binding    = 0,
      .stride     = sizeof(Vertex),
      .inputRate  = vk::VertexInputRate::eVertex
    };
  }

  static vk::VertexInputAttributeDescription attributeDescription()
  {
    return vk::VertexInputAttributeDescription{
      .location = 0,
      .binding  = 0,
      .format   = vk::Format::eR32G32Sfloat,
      .offset   = offsetof(Vertex, position)
    };
  }
};

struct ProjectionData{
  alignas(16) glm::mat4 model = glm::mat4(1.0f);
  alignas(16) glm::mat4 view = glm::mat4(1.0f);
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
};

struct PhysicalProperties
{
  alignas(16) glm::vec3 position = { 0.0f, 0.0f, 0.0f };
  alignas(16) glm::vec3 momentum = { 0.0f, 0.0f, 0.0f };
  alignas(4) float energy = 0.0f;
  alignas(4) float mass = 1.0f;
  alignas(4) float charge = 1.0f;
};

class DeviceAllocation : public Singular
{
  friend class Object;
  friend class Renderer;
  
  public:
    DeviceAllocation(const Device *, const std::vector<Vertex>&, const std::vector<unsigned int>&);

    ~DeviceAllocation();

    void allocate(std::size_t);
    void createDescriptors(std::size_t, const std::vector<vk::raii::DescriptorSetLayout>&);
    
    template <typename T>
    unsigned long addAllocation(T&, std::string);

    void updateProjection(ProjectionData&);
    void updateProperties(PhysicalProperties&);

    template <typename T>
    void updateBuffer(unsigned long, T&);

  private:
    unsigned int findMemoryIndex(unsigned int, vk::MemoryPropertyFlags);

    void createBuffers();
    
  private:
    const Device * device = nullptr;

    const unsigned long vertexSize, indexSize;
    const Vertex * vertexData;
    const unsigned int * indexData;

    vk::raii::DeviceMemory vk_modelMemory = nullptr;
    vk::raii::Buffer vk_vertexBuffer = nullptr;
    vk::raii::Buffer vk_indexBuffer = nullptr;

    vk::raii::DeviceMemory vk_projectionMemory = nullptr;
    vk::raii::Buffer vk_projectionBuffer = nullptr;
    void * projectionMapping = nullptr;

    vk::raii::DeviceMemory vk_propertiesMemory = nullptr;
    vk::raii::Buffer vk_propertiesBuffer = nullptr;
    void * propertiesMapping = nullptr;

    std::vector<vk::raii::DeviceMemory> vk_otherMemories;
    std::vector<vk::raii::Buffer> vk_otherBuffers;
    std::vector<void *> otherMappings;

    std::unordered_map<std::string, unsigned long> objectSizes;
    std::vector<std::string> objectTypes;

    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    std::vector<std::vector<vk::raii::DescriptorSet>> vk_projectionSets;
    std::vector<std::vector<vk::raii::DescriptorSet>> vk_propertiesSets;
    std::vector<std::vector<vk::raii::DescriptorSet>> vk_otherSets;
};

} // namespace pecs

#endif // pecs_core_deviceallocation_hpp