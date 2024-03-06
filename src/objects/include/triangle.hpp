/*
 *  PECS::objects - triangle.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  3/5/24
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
    Triangle(GraphicsShaders, glm::vec3, float, glm::vec3);

    ~Triangle() = default;
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_triangle_hpp