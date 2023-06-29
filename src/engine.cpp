/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/29/23
*/

#include "include/engine.hpp"
#include <iostream>

namespace pecs
{

Engine::Engine(const ApplicationInfo* appInfo)
{
    // window = new Window(appInfo->windowWidth,
    //                     appInfo->windowHeight,
    //                     appInfo->windowTitle);

    std::cout << "Hello World";
}

Engine::~Engine()
{
    // delete window;
}

}