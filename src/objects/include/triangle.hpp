/*
 *  PECS::objects - triangle.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/18/24
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
    Triangle(ShaderPaths, glm::vec3 p = { 0.0f, 0.0f, 0.0f }, RotationInfo r = {0.0f, { 0.0f, 0.0f, 1.0f }});

    ~Triangle() = default;
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_triangle_hpp