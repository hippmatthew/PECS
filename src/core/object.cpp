/*
 *  PECS::core - object.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/26/24
 *  Updated:  3/5/24
 */

#include "src/core/include/object.hpp"

namespace pecs
{

Object::Object(Object&& o)
{
  graphics = o.graphics;
  o.graphics = nullptr;
}

Object::~Object()
{
  delete graphics;
}

Object& Object::operator = (Object&& o)
{
  if (this == &o) return *this;

  if (graphics != nullptr) delete graphics;

  graphics = o.graphics;
  o.graphics = nullptr;

  return *this;
}

} // namespace pecs

namespace std
{

pecs::Object&& move(pecs::Object& o)
{
  return static_cast<pecs::Object&&>(o);
}

} // namespace std