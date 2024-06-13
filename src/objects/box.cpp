#include "src/objects/include/box.hpp"

namespace pecs
{
namespace objects
{

Box::Box(GraphicsShaders paths, glm::vec3 position, float angle, glm::vec3 axis)
{
  std::vector<Vertex> vertices = {
    {{0.5f, 0.5f, 0.5f}},
    {{0.5f, 0.5f, -0.5f}},
    {{0.5f, -0.5f, 0.5f}},
    {{0.5f, -0.5f, -0.5f}},
    {{-0.5f, 0.5f, 0.5f}},
    {{-0.5f, 0.5f, -0.5f}},
    {{-0.5f, -0.5f, 0.5f}},
    {{-0.5f, -0.5f, -0.5f}}
  };

  std::vector<unsigned int> indices = {
    1, 0, 2, 2, 3, 1,
    1, 5, 4, 4, 0, 1,
    1, 3, 7, 7, 5, 1,
    6, 2, 3, 3, 7, 6,
    6, 4, 5, 5, 7, 6,
    6, 2, 0, 0, 4, 6
  };

  graphics = new GraphicsComponent(paths, vertices, indices);

  translate(position)
    .rotate(angle, axis);
}

Box& Box::translate(glm::vec3 position)
{
  graphics->translateModel(position);
  return *this;
}

Box& Box::rotate(float angle, glm::vec3 axis)
{
  graphics->rotateModel(angle, axis);
  return *this;
}

} // namespace pecs::objects
} // namespace pecs