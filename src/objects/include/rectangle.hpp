/*
 *  PECS::objects - rectangle.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/9/24
 *  Updated:  2/18/24
 */

#ifndef pecs_objects_rectangle_hpp
#define pecs_objects_rectangle_hpp

#include "src/core/include/object.hpp"

namespace pecs
{
namespace objects
{

class Rectangle : public Object
{
  public:
    Rectangle(ShaderPaths, glm::vec3 t = { 0.0f, 0.0f, 0.0f }, RotationInfo r = { 0.0f, { 0.0f, 0.0f, 1.0f }});

    ~Rectangle() = default;
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_rectangle_hpp