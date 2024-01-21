/*
 *  PECS - pipeline.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#ifndef pecs_pipeline_hpp
#define pecs_pipeline_hpp

#include "src/include/singular.hpp"
#include "src/include/device.hpp"

#include <fstream>
#include <vector>

namespace pecs
{

typedef std::optional<std::string> ShaderPath;

struct ShaderPaths
{
  ShaderPath vertex;
  ShaderPath fragment;
  ShaderPath compute;
};

class Pipeline : public Singular
{
  public:
    Pipeline(const ShaderPaths& s) : shaderPaths(s) {}

    virtual ~Pipeline() = default;
  
  protected:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStageCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

  protected:
    ShaderPaths shaderPaths;

    vk::raii::Pipeline vk_pipeline = nullptr;
    vk::raii::PipelineLayout vk_layout = nullptr;
};

class GraphicsPipeline : public Pipeline
{
  public:
    GraphicsPipeline(const vk::raii::Device&, const vk::Extent2D&, const vk::Format&, const ShaderPaths&);

    ~GraphicsPipeline() = default;
};

} // namespace pecs

#endif // pecs_pipeline_hpp