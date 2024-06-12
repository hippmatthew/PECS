#ifndef pecs_core_component_hpp
#define pecs_core_component_hpp

#include "src/core/include/device.hpp"
#include "src/core/include/singular.hpp"

#ifndef pecs_include_glm
#define pecs_include_glm

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif // pecs_include_glm

#include <fstream>

namespace pecs
{

typedef std::optional<std::string> ShaderPath;
typedef std::pair<vk::Extent2D, vk::Format> ViewportInfo;
typedef std::vector<std::pair<vk::ShaderStageFlagBits, vk::raii::ShaderModule>> ShaderModules;
typedef std::vector<vk::PipelineShaderStageCreateInfo> ShaderInfos;

struct GraphicsShaders
{
  ShaderPath vertex;
  ShaderPath tesselation1;
  ShaderPath tesselation2;
  ShaderPath geometry;
  ShaderPath fragment;

  std::vector<ShaderPath> array() const
  {
    return { vertex, tesselation1, tesselation2, geometry, fragment };
  }
};

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

struct Camera
{
  alignas(16) glm::mat4 view = glm::mat4(1.0f);
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
};

class Component : public Singular
{
  public:
    Component() = default;

    virtual ~Component() = default;

    virtual void initialize() = 0;
};

class PipelineComponent : public Component
{
  public:
    PipelineComponent() = default;

    virtual ~PipelineComponent() = default;

    virtual void initialize() {}
  
  protected:
    std::vector<char> readShader(std::string) const;
    unsigned int findMemoryIndex(const vk::raii::PhysicalDevice&, unsigned int, vk::MemoryPropertyFlags) const;

    virtual void createPipeline(const vk::raii::Device&) = 0;

  protected:
    vk::raii::PipelineLayout vk_pipelineLayout = nullptr;
    vk::raii::Pipeline vk_pipeline = nullptr; 
};

class GraphicsComponent : public PipelineComponent
{
  friend class Renderer;
  
  public:
    GraphicsComponent(const GraphicsShaders&, const std::vector<Vertex>&, const std::vector<unsigned int>&);

    ~GraphicsComponent() = default;

    GraphicsComponent& translateModel(glm::vec3);
    GraphicsComponent& rotateModel(float, glm::vec3);

    void initialize(const Device&, const ViewportInfo&, const vk::raii::Buffer&, unsigned long);

  private:
    ShaderModules createShaderModules(const vk::raii::Device&) const;
    ShaderInfos shaderInfos(const ShaderModules&) const;

    void createPipeline(const vk::raii::Device&);
    void allocateBuffers(const Device&);
    void createDescriptorSets(const vk::raii::Device&, unsigned long, const vk::raii::Buffer&);

  private:
    const GraphicsShaders paths;
    ViewportInfo i_viewport;
    alignas(16) glm::mat4 model = glm::mat4(1.0f);

    const std::vector<Vertex> vertices;
    const std::vector<unsigned int> indices;

    std::vector<vk::raii::DescriptorSetLayout> vk_descriptorLayouts;

    vk::raii::DeviceMemory vk_objectMemory = nullptr;
    vk::raii::Buffer vk_vertexBuffer = nullptr;
    vk::raii::Buffer vk_indexBuffer = nullptr;

    vk::raii::DeviceMemory vk_globalMemory = nullptr;
    vk::raii::Buffer vk_cameraBuffer = nullptr;
    void * cameraMapping = nullptr;

    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    std::vector<std::vector<vk::raii::DescriptorSet>> vk_descriptorSets;
};

} // namespace pecs

#endif // pecs_core_component_hpp