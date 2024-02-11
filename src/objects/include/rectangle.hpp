/*
 *  PECS::objects - rectangle.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/9/24
 *  Updated:  2/11/24
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
    Rectangle(std::array<float, 2>, ShaderPaths, glm::vec2 p = { 0.0f, 0.0f });

    ~Rectangle() = default;
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_rectangle_hpp