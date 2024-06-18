#include "src/core/include/graphics.hpp"

#define VECS_MODEL_TAG "model"

namespace vecs
{

GraphicsComponent::GraphicsBuilder::GraphicsBuilder(Material mat)
{
  p_component = new GraphicsComponent;
  p_component->material = mat;
}

GraphicsComponent::GraphicsBuilder::~GraphicsBuilder()
{
  delete p_component;
}

GraphicsComponent::GraphicsBuilder& GraphicsComponent::GraphicsBuilder::add_model(std::string path)
{
  if (!p_component->vertices.empty() || !p_component->vertices.empty()) return *this;

  return *this;
}

GraphicsComponent::GraphicsBuilder& GraphicsComponent::GraphicsBuilder::add_model(
  std::vector<VertexData>& vertices,
  std::vector<unsigned int>& indices
)
{
  if (!p_component->vertices.empty() || !p_component->vertices.empty()) return *this;

  p_component->vertices = vertices;
  p_component->indices = indices;

  return *this;
}

GraphicsComponent::GraphicsBuilder& GraphicsComponent::GraphicsBuilder::add_uniform_buffer(std::shared_ptr<Custom> data)
{
  if (p_component->uniformMap.find(data->tag) == p_component->uniformMap.end() ) return *this;

  p_component->uniformMap.emplace(std::make_pair(data->tag, data));
  return *this;
}

GraphicsComponent::GraphicsBuilder& GraphicsComponent::GraphicsBuilder::add_push_constant(std::shared_ptr<Custom> data)
{
  if (p_component->pushConstantMap.find(data->tag) == p_component->uniformMap.end()) return *this;

  p_component->pushConstantMap.emplace(std::make_pair(data->tag, data));
  return *this;
}

GraphicsComponent& GraphicsComponent::GraphicsBuilder::build() const
{
  return *p_component;
}

GraphicsComponent& GraphicsComponent::translate(glm::vec3 translation)
{
  model = glm::translate(model, translation);
  return *this;
}

GraphicsComponent& GraphicsComponent::rotate(float angle, glm::vec3 axis)
{
  model = glm::rotate(model, glm::radians(angle), axis);
  return *this;
}

} // namespace vecs