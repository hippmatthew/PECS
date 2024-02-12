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

Rectangle::Rectangle(std::array<float, 2> lengths, ShaderPaths s, glm::vec2 p) : Object(s, p)
{
  glm::vec2 centering = { 0.5 * lengths[0], 1 - 0.5 * lengths[1] };
  
  vertices = {
    {{0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    {{0.0f, lengths[1]}, {0.0f, 1.0f, 0.0f}},
    {{lengths[0], lengths[1]}, {0.0f, 0.0f, 1.0f}},
    {{lengths[0], 0.0f}, {0.0f, 1.0f, 0.0f}}
  };

  for (auto& vertex : vertices)
    vertex.position += p - centering;

  indices = { 0, 1, 2, 2, 3, 0 };
}

} // namespace pecs::objects
} // namespace pecs