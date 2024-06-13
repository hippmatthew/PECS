/*
 *  PECS - pecs-objects.hpp
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/18/24
 */

#ifndef pecs_objects_hpp
#define pecs_objects_hpp

#include "pecs-core.hpp"

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

class Rectangle : public Object
{
  public:
    Rectangle(GraphicsShaders, glm::vec3, float, glm::vec3);

    ~Rectangle() = default;
};;

class Box : public Object
{
  public:
    Box(GraphicsShaders, glm::vec3, float, glm::vec3);

    ~Box() = default;

    Box& translate(glm::vec3);
    Box& rotate(float, glm::vec3);
};

} // namespace pecs::objects
} // namespace pecs

#endif // pecs_objects_hpp