/*
 *  PECS - engine.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/4/24
 */

#ifndef pecs_engine_hpp
#define pecs_engine_hpp

#include "src/include/loop.hpp"
#include "src/include/renderer.hpp"

namespace pecs
{

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    ~Engine();

    const ViewportInfo viewportInfo() const;

    void run(Loop&);
    void addObject(const ShaderPaths&, unsigned int);
    
  private:
    void initialize();
    void createInstance();
    void createSyncObjects();

  private:
    Settings::Engine settings;
    std::vector<const char *> layers;
    
    GUI * gui = nullptr;
    Device * device = nullptr;
    Renderer * renderer = nullptr;
    std::vector<Object *> objects;

    vk::raii::Instance vk_instance = nullptr;
    vk::raii::Semaphore vk_imageSemaphore = nullptr;
    vk::raii::Semaphore vk_renderSemaphore = nullptr;
    vk::raii::Fence vk_flightFence = nullptr;
};

} // namespace pecs

#endif // pecs_engine_hpp