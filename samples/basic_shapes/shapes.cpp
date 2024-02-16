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
    Simulation()
    {
      triangle = new pecs::objects::Triangle(
        {},
        0.0f,
        { "spv/default.vert.spv", "spv/default.frag.spv"}
      );
      triangle->translate({ 0.0f, 0.3f, 0.3f});

      rectangle = new pecs::objects::Rectangle(
        {},
        { "spv/default.vert.spv", "spv/default.frag.spv"}
      );
      rectangle->translate({ 0.3f, 0.0f, 0.3f });

      addObject(triangle);
      addObject(rectangle);
    }
    
    void Main()
    {
      triangle->rotate({glm::radians(270.0f) * deltaTime, {1.0f, 1.0f, 1.0f}});
      rectangle->rotate({glm::radians(30.0f) * deltaTime, {1.0f, 1.0f, 1.0f}});
    }

    ~Simulation()
    {
      delete triangle;
      delete rectangle;
    }

  private:
    pecs::objects::Triangle * triangle = nullptr;
    pecs::objects::Rectangle * rectangle = nullptr;
};

int main()
{
  Simulation sim;

  sim.run();
}