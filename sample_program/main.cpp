/*
 *  PECS - sample program
 *  Author:     Matthew Hipp
 *  Created:    6/29/23
 *  Updated:    10/31/23
 */

#include "../include/pecs/core.hpp"
#include "../include/pecs/objects.hpp"

class Simulation : public pecs::Engine::Main
{
    // any extra data / functions can be expressed here

    public:
        Simulation(const pecs::Engine& engine) : Main(engine) 
        {
            pecs::ShaderPaths sp{ .vertex   = "../spv/triangle.vert.spv",
                                  .fragment = "../spv/triangle.frag.spv" };
            
            objects.push_back(pecs::Triangle(sp));
        }

        void operator()()
        {
            // required operator main loop
        }
};  

int main()
{
    // Write out the initialization info
    pecs::InitializationInfo initInfo{ .applicationName     = "PECS Sample Program",
                                       .applicationVersion  = VK_MAKE_API_VERSION(0, 1, 0, 0),
                                       .windowTitle         = "PECS Test",
                                       .windowWidth         = 600,
                                       .windowHeight        = 600 };
    
    // Create the engine object and initalize it
    pecs::Engine engine;
    engine.initialize(&initInfo);

    Simulation sim(engine);

    engine.run(sim);
}