/*
 *  PECS::objects - triangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/18/24
 */

#include "src/objects/include/triangle.hpp"

namespace pecs
{
namespace objects
{

Triangle::Triangle(ShaderPaths s,  glm::vec3 p, RotationInfo r) : Object(s)
{ 
  vertices = {
    {{0.5f, 0.35f, 0.0f}},
    {{-0.5f, 0.35f, 0.0f}},
    {{0.0f, -0.65f, 0.0f}}
  };
  
  indices = { 0, 1, 2 };

  translate(p);
  rotate(r);
}

} // namespace pecs::objects
} // namespace pecs