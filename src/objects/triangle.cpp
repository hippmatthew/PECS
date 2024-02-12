/*
 *  PECS::objects - triangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/11/24
 */

#include "src/objects/include/triangle.hpp"

#include <math.h>

namespace pecs
{
namespace objects
{

Triangle::Triangle(std::array<float, 2> sides, float theta1, ShaderPaths s,  glm::vec2 p) : Object(s, p)
{
  float side3 = sqrt(sides[0] * sides[0] + sides[1] * sides[1] - 2 * sides[0] * sides[1] * cos(theta1));
  float sin_theta2 = sides[1] * sin(theta1) / side3;
  float cos_theta2 = (sides[0] - sides[1] * cos(theta1)) / side3;
  
  float h = sides[0] * sin_theta2;
  float x2 = sides[0] * cos_theta2;
  float x3 = sides[1] * (cos(theta1) * cos_theta2 - sin(theta1) * sin_theta2);

  glm::vec2 centering = { 0.0f, h - (x2 * sin_theta2 / (1 + cos_theta2)) };
  
  vertices = {
    {{0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    {{-x2, h}, {1.0f, 0.0f, 0.0f}},
    {{x3, h}, {0.0f, 0.0f, 1.0f}}
  };
  
  for (auto& vertex : vertices)
    vertex.position += p - centering;

  indices = { 0, 1, 2 };
}

} // namespace pecs::objects
} // namespace pecs