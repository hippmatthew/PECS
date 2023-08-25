/*
 *  PECS - sample program
 *  Author:     Matthew Hipp
 *  Created:    6/29/23
 *  Updated:    7/18/23
 */

#include "../include/pecs.hpp"

int main()
{
    // Write out the initialization info
    pecs::InitializationInfo initInfo{ .engineName          = "PECS",
                                       .engineVersion       = VK_MAKE_API_VERSION(0, 1, 0, 0),
                                       .applicationName     = "PECS Sample Program",
                                       .applicationVersion  = VK_MAKE_API_VERSION(0, 1, 0, 0),
                                       .windowTitle         = "PECS Test",
                                       .windowWidth         = 600,
                                       .windowHeight        = 600,
                                       .enableDebugManager  = true };
    
    // Create the engine object and initalize it
    pecs::Engine engine;
    engine.initialize(&initInfo);

    // Main loop
    while(engine.isActive())
    {
        engine.getEvents();
    }

    return 0;
}