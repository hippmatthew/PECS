/*
 *  PECS::samples::triangle - triangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/4/24
 */

#include "pecs.hpp"

#include <iostream>

class Simulation : public pecs::Engine
{
  public:
    void Main()
    {
      if (++counter % 1000 == 0)
        std::cout << "running... (" << counter / 1000 << ")\n"; 
    }

  private:
    std::size_t counter = 0;
};

int main()
{
  Simulation sim;

  pecs::ShaderPaths shaderPaths{
    .vertex = "triangles/triangle1.vert.spv",
    .fragment = "triangles/triangle.frag.spv"
  };
  sim.addObject(shaderPaths, 3);

  pecs::ShaderPaths shaderPaths2{
    .vertex = "triangles/triangle2.vert.spv",
    .fragment = "triangles/triangle.frag.spv"
  };
  sim.addObject(shaderPaths2, 3);

  sim.run();
}