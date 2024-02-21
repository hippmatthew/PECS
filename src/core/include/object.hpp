/*
 *  PECS::core - object.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/20/24
 */

#ifndef pecs_core_object_hpp
#define pecs_core_object_hpp

#include "src/core/include/device.hpp"
#include "src/core/include/singular.hpp"
#include "src/core/include/deviceallocation.hpp"

#include <fstream>

namespace pecs
{

typedef std::optional<std::string> ShaderPath;
typedef std::pair<vk::Extent2D, vk::Format> ViewportInfo;

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

    ~Object();

    Object& operator = (const Object&);
    Object& operator = (Object&&);
    
    Object& addEnergy(float);
    Object& addMomentum(glm::vec3);
    Object& addMass(float);
    Object& addCharge(float);
    Object& translate(glm::vec3);
    Object& rotate(RotationInfo);

    const glm::vec3& momentum() const;
    const glm::vec3& position() const;
    const float& mass() const;

    template <typename T>
    void updateCustomProperty(unsigned long, T&);

    void clean(bool removeAllocation = true);
    
  private:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

    void createGraphicsPipeline(const vk::raii::Device&, const ViewportInfo&);

  private:
    bool valid = false;
    bool hasTransformed = false;

    glm::mat4 nextTransformation = glm::mat4(1.0f);

    std::vector<vk::raii::DescriptorSetLayout> vk_descriptorLayouts;
    vk::raii::PipelineLayout vk_graphicsLayout = nullptr;
    vk::raii::Pipeline vk_graphicsPipeline = nullptr;

  protected:
    DeviceAllocation * allocation = nullptr;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    ShaderPaths shaderPaths;
    
    ProjectionData projection;
    PhysicalProperties properties;
};

} // namespace pecs

#endif // pecs_core_object_hpp