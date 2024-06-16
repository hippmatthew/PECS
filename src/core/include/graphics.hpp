#ifndef vecs_core_graphics_hpp
#define vecs_core_graphics_hpp

#include "src/core/include/material.hpp"

#ifndef vecs_include_vulkan
#define vecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // vecs_include_vulkan

#ifndef vecs_include_glm
#define vecs_include_glm

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif // vecs_include_glm

#include <map>
#include <memory>
#include <string>

namespace vecs
{

struct VertexData
{
  static vk::VertexInputBindingDescription binding()
  {
    return vk::VertexInputBindingDescription{
      .binding  = 0,
      .stride     = sizeof(VertexData),
      .inputRate  = vk::VertexInputRate::eVertex
    };
  }

  static vk::VertexInputAttributeDescription attribute()
  {
    return vk::VertexInputAttributeDescription{
      .location = 0,
      .binding  = 0,
      .format   = vk::Format::eR32G32B32A32Sfloat,
      .offset   = __offsetof(VertexData, v_position)
    };
  }

  glm::vec3 v_position;
};

class Custom
{
  public:
    Custom() = default;
    Custom(const Custom&) = default;
    Custom(Custom&&) = default;

    ~Custom() = default;

    Custom& operator = (const Custom&) = default;
    Custom& operator = (Custom&&) = default;
  
    virtual unsigned int size() = 0;
  
  protected:
    std::string tag;
    unsigned long index;

  friend class GraphicsComponent;
  friend class Renderer;
};

template <typename T>
class Data : public Custom
{
  public:
    Data(std::string s, T t) : data(t) { tag = s; }
    Data(const Data&) = default;
    Data(Data&&) = default;

    ~Data() = default;

    Data& operator = (const Data&) = default;
    Data& operator = (Data&&) = default;

    unsigned int size() { return sizeof(T); };

  private:
    T data;

  friend class GraphicsComponent;
  friend class Renderer;
};

class GraphicsComponent
{
  friend class renderer;
  
  public:
    class GraphicsBuilder
    {
      public: 
        GraphicsBuilder(std::string, Material);
        GraphicsBuilder(const GraphicsBuilder&) = delete;
        GraphicsBuilder(GraphicsBuilder&&) = delete;

        ~GraphicsBuilder();

        GraphicsBuilder& operator = (const GraphicsBuilder&) = delete;
        GraphicsBuilder& operator = (GraphicsBuilder&&) = delete;

        GraphicsBuilder& add_model(std::string);
        GraphicsBuilder& add_model(std::vector<VertexData>&, std::vector<unsigned int>&);
        GraphicsBuilder& add_uniform_buffer(std::shared_ptr<Custom>);
        GraphicsBuilder& add_push_constant(std::shared_ptr<Custom>);

        GraphicsComponent& build() const;

      private:
        GraphicsComponent * p_component;
    };
  
  public:
    GraphicsComponent() = default;
    GraphicsComponent(const GraphicsComponent&) = default;
    GraphicsComponent(GraphicsComponent&&) = default;

    ~GraphicsComponent() = default;

    GraphicsComponent& operator = (const GraphicsComponent&) = default;
    GraphicsComponent& operator = (GraphicsComponent&&) = default;

    GraphicsComponent& translate(glm::vec3);
    GraphicsComponent& rotate(float, glm::vec3);
  
  private:
    std::string pipelineTag;
    Material material;

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 model = glm::mat4(1.0f);

    std::map<std::string, std::shared_ptr<Custom>> uniforms;
    std::map<std::string, std::shared_ptr<Custom>> pushConstants;

  friend class GraphicsBuilder;
};

} // namespace vecs

#endif // vecs_graphics_hpp