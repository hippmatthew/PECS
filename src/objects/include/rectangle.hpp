/*
 *  PECS::objects - rectangle.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/9/24
 *  Updated:  3/5/24
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
    Rectangle(GraphicsShaders, glm::vec3, float, glm::vec3);

    ~Rectangle() = default;
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_rectangle_hpp