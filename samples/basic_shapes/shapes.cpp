/*
 *  PECS::samples::triangle - triangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/11/24
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
    .vertex = "spv/default.vert.spv",
    .fragment = "spv/default.frag.spv"
  };
  
  // pecs::objects::Triangle triangle({ 0.3, 0.2, 0.1}, shaderPaths);
  pecs::objects::Rectangle rectangle({ 0.3, 0.3 }, shaderPaths);

  // sim.addObject(triangle);
  sim.addObject(rectangle);

  sim.run();
}