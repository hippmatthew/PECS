/*
 *  PECS::samples::triangle - triangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/4/24
 */

#include "pecs.hpp"

#include <iostream>
#include <utility>

class Simulation : public pecs::Loop
{
  public:
    void operator () ()
    {
      if (++counter % 10000 == 0)
        std::cout << "running... (" << counter / 10000 << ")\n";
    }

  private:
    std::size_t counter = 0;
};

int main()
{
  pecs::Engine engine;
  Simulation sim;

  pecs::ShaderPaths shaderPaths{
    .vertex = "triangle/triangle.vert.spv",
    .fragment = "triangle/triangle.frag.spv"
  };
  engine.addObject(shaderPaths, 3);

  pecs::ShaderPaths shaderPaths2{
    .vertex = "triangle/triangle2.vert.spv",
    .fragment = "triangle/triangle.frag.spv"
  };
  engine.addObject(shaderPaths2, 3);

  engine.run(sim);
}