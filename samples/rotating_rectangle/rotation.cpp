/*
 *  PECS::samples::rotating_rectangle - rectangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  2/13/24
 *  Updated:  2/15/24
 */

#include "pecs-objects.hpp"

#include <math.h>
#include <iostream>

class Simulation : public pecs::Engine
{
  public:
    Simulation()
    {
      rectangle = new pecs::objects::Rectangle({}, { "spv/default.vert.spv", "spv/default.frag.spv" });
      addObject(rectangle);

      rectangle->translate({ 0.0, 0.0, -0.5 });
    }
    
    ~Simulation()
    {
      delete rectangle;
    }

    void Main()
    {
      elapsedTime += deltaTime;
  
      rectangle->rotate({ angularVelocity * deltaTime, rotationAxis });
      rectangle->translate({ 0.0f, 0.0f, 0.01 * sin(elapsedTime) });
    }

  private:
    pecs::objects::Rectangle * rectangle = nullptr;
    float angularVelocity = glm::radians(45.0f);
    glm::vec3 rotationAxis = { 0.0f, 0.0f, 1.0f };
    float elapsedTime = 0;
};

int main()
{
  Simulation sim;

  sim.run();
}