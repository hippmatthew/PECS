/*
 *  PECS - loop.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/26/24
 *  Updated:  1/26/24
 */

#include "src/include/loop.hpp"

namespace pecs
{

Loop::~Loop()
{
  for (auto * object : objects)
    delete object;
}

std::vector<Object *>& Loop::objectData()
{
  return objects;
}

void Loop::addObject(const Device& device, const ViewportInfo& i_viewport, const ShaderPaths& shaderPaths)
{
  objects.emplace_back(new Object(device.logical(), i_viewport, shaderPaths));
}

} // namespace pecs