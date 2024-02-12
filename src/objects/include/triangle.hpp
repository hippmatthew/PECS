/*
 *  PECS::objects - triangle.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/11/24
 */

#ifndef pecs_objects_triangle_hpp
#define pecs_objects_triangle_hpp

#include "src/core/include/object.hpp"

namespace pecs
{
namespace objects
{

class Triangle : public Object
{
  public:
    Triangle(std::array<float, 2>, float, ShaderPaths, glm::vec2 p = {0.0f, 0.0f});

    ~Triangle() = default;
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_triangle_hpp