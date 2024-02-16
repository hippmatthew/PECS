/*
 *  PECS::core - object.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/15/24
 */

#ifndef pecs_core_object_hpp
#define pecs_core_object_hpp

#include "src/core/include/device.hpp"
#include "src/core/include/singular.hpp"

#ifndef pecs_include_glm
#define pecs_include_glm

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#endif // pecs_include_glm

#include <fstream>

namespace pecs
{

typedef std::optional<std::string> ShaderPath;
typedef std::pair<vk::Extent2D, vk::Format> ViewportInfo;

struct Vertex
{
  glm::vec2 position;
  glm::vec3 color;

  static vk::VertexInputBindingDescription bindingDescription()
  {
    return vk::VertexInputBindingDescription{
      .binding    = 0,
      .stride     = sizeof(Vertex),
      .inputRate  = vk::VertexInputRate::eVertex
    };
  }

  static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions()
  {
    return std::array<vk::VertexInputAttributeDescription, 2>{
      vk::VertexInputAttributeDescription{
        .location = 0,
        .binding  = 0,
        .format   = vk::Format::eR32G32Sfloat,
        .offset   = offsetof(Vertex, position)
      },
      vk::VertexInputAttributeDescription{
        .location = 1,
        .binding  = 0,
        .format   = vk::Format::eR32G32B32Sfloat,
        .offset   = offsetof(Vertex, color)
      }
    };
  }
};

struct UniformBufferObject
{
  glm::mat4 model = glm::mat4(1.0f);
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
};

struct RotationInfo
{
  float angle = 0.0f;
  glm::vec3 axis = { 0.0f, 0.0f, 1.0f };
};

struct ShaderPaths
{
  ShaderPath vertex;
  ShaderPath fragment;
  ShaderPath compute;
};

class Object
{
  friend class Engine;
  friend class Renderer;

  public:
    Object(ShaderPaths);
    Object(const Object&);
    Object(Object&&);

    ~Object() = default;

    Object& operator = (const Object&);
    Object& operator = (Object&&);

    void translate(glm::vec3);
    void rotate(RotationInfo);

    void clean();
    
  protected:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

    void createGraphicsPipeline(const vk::raii::Device&, const ViewportInfo&);

  protected:
    glm::vec3 position;
    glm::mat4 nextTransformation = glm::mat4(1.0f);
    ShaderPaths shaderPaths;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    bool hasTransformed = false;

    UniformBufferObject objectData;
    
    vk::raii::DescriptorSetLayout vk_descriptorLayout = nullptr;
    
    vk::raii::PipelineLayout vk_graphicsLayout = nullptr;
    vk::raii::Pipeline vk_graphicsPipeline = nullptr;
};

} // namespace pecs

#endif // pecs_core_object_hpp