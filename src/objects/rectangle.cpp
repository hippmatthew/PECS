/*
 *  PECS::objects - rectangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/11/24
 *  Updated:  2/18/24
 */

#include "src/objects/include/rectangle.hpp"

namespace pecs
{
namespace objects
{

Rectangle::Rectangle(ShaderPaths s, glm::vec3 p, RotationInfo r) : Object(s)
{ 
  vertices = {
    {{-0.5f, -0.5f, -0.5f}},
    {{0.5, -0.5f, -0.5f}},
    {{0.5f, 0.5f, -0.5f}},
    {{-0.5f, 0.5f, -0.5f}}
  }; 

  indices = { 0, 1, 2, 2, 3, 0 };

  translate(p);
  rotate(r);
}

} // namespace pecs::objects
} // namespace pecs