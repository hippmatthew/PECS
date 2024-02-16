/*
 *  PECS::objects - triangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/11/24
 */

#include "src/objects/include/triangle.hpp"

namespace pecs
{
namespace objects
{

Triangle::Triangle(std::array<float, 2> sides, float theta1, ShaderPaths s,  glm::vec3 p, RotationInfo r) : Object(s)
{ 
  vertices = {
    {{0.0f, -0.65f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.35f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.35f}, {0.0f, 0.0f, 1.0f}}
  };
  
  indices = { 2, 1, 0 };

  translate(p);
  rotate(r);
}

} // namespace pecs::objects
} // namespace pecs