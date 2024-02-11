/*
 *  PECS - pecs-objects.hpp
 *  Author:   Matthew Hipp
 *  Created:  2/8/24
 *  Updated:  2/9/24
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
    Triangle(std::array<float, 2>, ShaderPaths, glm::vec2 p = {0.0f, 0.0f});

    ~Triangle() = default;
};

} // namespace objects
} // namespace pecs

#endif // pecs_objects_hpp