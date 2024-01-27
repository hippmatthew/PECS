/*
 *  PECS::samples::triangle - triangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#include "pecs.hpp"

#include <iostream>

class Simulation : public pecs::Loop
{
  public:
    Simulation(const pecs::Engine& e)
    {
      // pecs::ShaderPaths s{
      //   .vertex = "./triangle/triangle.vert.spv",
      //   .fragment = "./triangle/triangle.frag.spv"
      // };

      // addObject(e.getDevice(), e.viewportInfo(), s);
    }

    void operator () ()
    {
      if (++counter == 25)
      {
        std::cout << "running...\n";
        counter = 0;
      }
    }

  private:
    int counter = 0;
};

int main()
{
  pecs::Engine engine;
  Simulation sim(engine);

  engine.run(sim);
}