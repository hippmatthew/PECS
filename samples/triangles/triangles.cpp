/*
 *  PECS::samples::triangle - triangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/9/24
 */

#include "pecs-objects.hpp"

#include <iostream>

class Simulation : public pecs::Engine
{
  public:
    void Main()
    {
      if (++counter % 100 == 0)
      {
        
      }
    }

  private:
    std::size_t counter = 0;
};

int main()
{
  Simulation sim;
  
  pecs::ShaderPaths shaderPaths{
    .vertex = "triangles/triangle.vert.spv",
    .fragment = "triangles/triangle.frag.spv"
  };
  
  pecs::objects::Triangle triangle1({ M_PI / 3, M_PI / 6 }, shaderPaths, { 0.3f, 0.2f });
  pecs::objects::Triangle triangle2({ M_PI / 2, M_PI / 4}, shaderPaths, { -0.9f, -0.6f });
  pecs::objects::Triangle triangle3({ M_PI / 15, M_PI / 7 }, shaderPaths, { 0.2f, -0.9f });

  sim.addObject(triangle1);
  sim.addObject(triangle2);
  sim.addObject(triangle3);

  sim.run();
}