#ifndef pecs_objects_box_hpp
#define pecs_objects_box_hpp

#include "src/core/include/object.hpp"

namespace pecs
{
namespace objects
{

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

#endif // pecs_objects_box_hpp