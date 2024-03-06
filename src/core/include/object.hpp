/*
 *  PECS::core - object.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  3/5/24
 */

#ifndef pecs_core_object_hpp
#define pecs_core_object_hpp

#include "src/core/include/component.hpp"

namespace pecs
{

class Object
{
  friend class Engine;
  friend class Renderer;

  public:
    Object() = default;
    Object(const Object&) = delete;
    Object(Object&&);

    ~Object();

    Object& operator = (const Object&) = delete;
    Object& operator = (Object&&);

  protected:
    GraphicsComponent * graphics = nullptr;
};

} // namespace pecs

namespace std
{

pecs::Object&& move(pecs::Object&);

} // namespace pecs

#endif // pecs_core_object_hpp