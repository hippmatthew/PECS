/*
 *  PECS::objects - rectangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/11/24
 *  Updated:  3/5/24
 */

#include "src/objects/include/rectangle.hpp"

namespace pecs
{
namespace objects
{

Rectangle::Rectangle(GraphicsShaders paths, glm::vec3 position, float angle, glm::vec3 axis)
{ 
  std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, -0.5f}},
    {{0.5, -0.5f, -0.5f}},
    {{0.5f, 0.5f, -0.5f}},
    {{-0.5f, 0.5f, -0.5f}}
  }; 

  std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

  graphics = new GraphicsComponent(paths, vertices, indices);

  graphics->translateModel(position)
              .rotateModel(glm::radians(angle), axis);
}

} // namespace pecs::objects
} // namespace pecs