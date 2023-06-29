/*
*   PECS - engine.cpp
*   Author:     Matthew Hipp
*   Created:    6/27/23
*   Updated:    6/29/23
*/

#include "include/engine.hpp"

namespace pecs
{

Engine::Engine() {}

Engine::~Engine()
{
    delete window;
}

bool Engine::IsActive() const
{ return !window->ShouldClose(); }

Window* Engine::GetWindow() const
{ return window; }

void Engine::Initialize(const InitializationInfo* initInfo)
{
    window = new Window(initInfo->windowWidth, initInfo->windowHeight, initInfo->windowTitle);
}

}