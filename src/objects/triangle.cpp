/*
 *  PECS::objects - triangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/9/24
 */

#include "src/objects/include/triangle.hpp"

#include <math.h>

namespace pecs
{
namespace objects
{

Triangle::Triangle(std::array<float, 2> angles, ShaderPaths s,  glm::vec2 p) : Object(s, p)
{
  glm::vec2 displacement;
  displacement.x = sin(2 * angles[0]) / (2 * sin(angles[0] + angles[1]));
  displacement.y = displacement.x * tan(angles[0] / 2);
  
  vertices = {
    {{0.0f, 1.0f}, {1.0, 0.0, 0.0}},
    {{1.0f, 1.0f}, {0.0, 1.0, 0.0}},
    {{displacement.x, displacement.x * sin(angles[0])}, {0.0f, 0.0f, 1.0f}}
  };

  for (int i = 0; i < 3; ++i)
    vertices[i].position += position - displacement;
    
  indices = { 0, 1, 2 };
}

} // namespace objects
} // namespace pecs