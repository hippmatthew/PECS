/*
 *  PECS::samples::triangle - triangle.cpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/20/24
 */

#include "pecs-objects.hpp"

#include <iostream>

class Simulation : public pecs::Engine
{
  public:
    Simulation()
    {
      rectangle = new pecs::objects::Rectangle(
        { "spv/fallingbox.vert.spv", "spv/fallingbox.frag.spv" },
        { 0.0f, 0.0f, 0.0f }
      );
      rectangle->rotate({ glm::radians(45.0f), {0.0f, 1.0f, 0.0f }})
        .translate({ -1 * h, 0.0f, 0.0f })
        .addEnergy(2 * g * h);

      addObject(rectangle);
    }
    
    void Main()
    {        
      rectangle->translate(rectangle->momentum() * deltaTime / rectangle->mass());
      
      if (rectangle->position()[0] + 0.5f >= h)
      {
        float reboundMomentumSquared = totalMomentum * totalMomentum - 2 * rectangle->mass() * loss;
        if (reboundMomentumSquared < 0)
          reboundMomentumSquared = 0;
        
        rectangle->addMomentum({ -1 * (glm::sqrt(reboundMomentumSquared) + totalMomentum) - g * deltaTime, 0.0f, 0.0f })
          .addEnergy(-1 * loss);

        totalMomentum += -1 * (glm::sqrt(reboundMomentumSquared) + totalMomentum) - g * deltaTime;
      }
        
      rectangle->addMomentum({ g * deltaTime, 0.0f, 0.0f });
      totalMomentum += g * deltaTime;
    }

    ~Simulation()
    {
      delete rectangle;
    }

  private:
    pecs::objects::Rectangle * rectangle = nullptr;
    float g = 0.981f;
    float h = 2.0f;
    float totalMomentum = 0.0f;
    float loss = 0.5f;
};

int main()
{
  Simulation sim;
  sim.run();
}