/*
 *  PECS - object.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/26/24
 */

#ifndef pecs_object_hpp
#define pecs_object_hpp

#include "src/include/device.hpp"
#include "src/include/singular.hpp"

#include <fstream>

namespace pecs
{

typedef std::optional<std::string> ShaderPath;
typedef std::pair<vk::Extent2D, vk::Format> ViewportInfo;

struct ShaderPaths
{
  ShaderPath vertex;
  ShaderPath fragment;
  ShaderPath compute;
};

class Object : public Singular
{
  public:
    Object(const vk::raii::Device&, const ViewportInfo&, const ShaderPaths&);

    ~Object() = default;

  private:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

    void createGraphicsPipeline(const vk::raii::Device&, const ViewportInfo&);

  protected:
    const ShaderPaths shaderPaths;

    vk::raii::PipelineLayout vk_graphicsLayout = nullptr;
    vk::raii::Pipeline vk_graphicsPipeline = nullptr;

    vk::raii::PipelineLayout vk_computeLayout = nullptr;
    vk::raii::Pipeline vk_computePipeline = nullptr;

    vk::raii::DescriptorSetLayout vk_descriptorLayout = nullptr;
    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    vk::raii::DescriptorSet vk_descriptorSet = nullptr;
};

} // namespace pecs

#endif // pecs_object_hpp