/*
 *  PECS - settings.cpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#include "src/include/settings.hpp"

namespace pecs
{

Settings::Settings(const Settings& s)
{
  engine = s.engine;
  gui = s.gui;
}

Settings& Settings::operator = (const Settings& s)
{
  if (this == &s) return *this;

  engine = s.engine;
  gui = s.gui;

  return *this;
}

} // namespace pecs