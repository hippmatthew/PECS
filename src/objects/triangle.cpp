/*
 *  PECS::objects - triangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  3/5/24
 */

#include "src/objects/include/triangle.hpp"

namespace pecs
{
namespace objects
{

Triangle::Triangle(GraphicsShaders paths,  glm::vec3 position, float angle, glm::vec3 axis)
{ 
  std::vector<Vertex> vertices = {
    {{0.5f, 0.35f, 0.0f}},
    {{-0.5f, 0.35f, 0.0f}},
    {{0.0f, -0.65f, 0.0f}}
  };
  
  std::vector<unsigned int> indices = { 0, 1, 2 };

  graphics = new GraphicsComponent(paths, vertices, indices);

  graphics->translateModel(position)
              .rotateModel(glm::radians(angle), axis);
}

} // namespace pecs::objects
} // namespace pecs