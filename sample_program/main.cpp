#include "../include/pecs.hpp"

int main()
{
    // Write out the initialization info
    pecs::InitializationInfo initInfo{ .applicationName     = "PECS Sample Program",
                                       .applicationVersion  = VK_MAKE_API_VERSION(0, 1, 0, 0),
                                       .windowWidth         = 600,
                                       .windowHeight        = 600,
                                       .windowTitle         = "PECS Test" };
    
    // Create the engine object and initalize it
    pecs::Engine engine;
    engine.initialize(&initInfo);

    // Main loop
    while(engine.isActive())
    {
        engine.getWindow()->pollEvents();
    }

    return 0;
}