/*
 *  PECS::objects - rectangle.cpp 
 *  Author:   Matthew Hipp
 *  Created:  2/11/24
 *  Updated:  2/11/24
 */

#include "src/objects/include/rectangle.hpp"

namespace pecs
{
namespace objects
{

Rectangle::Rectangle(std::array<float, 2> lengths, ShaderPaths s, glm::vec3 p, RotationInfo r) : Object(s)
{ 
  vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5, -0.5f}, {0.0f, 1.0f, 0.0f}}
  };

  indices = { 0, 3, 2, 2, 1, 0 };

  translate(p);
  rotate(r);
}

} // namespace pecs::objects
} // namespace pecs