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

Triangle::Triangle(std::array<float, 3> lengths, ShaderPaths s,  glm::vec2 p) : Object(s, p)
{
  vertices = {
    {{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
  };

  indices = { 0, 1, 2 };
}

} // namespace pecs::objects
} // namespace pecs