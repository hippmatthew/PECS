/*
 *  PECS - engine.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/7/24
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
    void initialize(const Settings&);
    void createInstance();
    void createSyncObjects();

  private:
    Settings::Engine settings;
    std::vector<const char *> layers;
    unsigned int frameIndex = 0;
    
    GUI * gui = nullptr;
    Device * device = nullptr;
    Renderer * renderer = nullptr;
    std::vector<Object *> objects;

    vk::raii::Instance vk_instance = nullptr;

    std::vector<vk::raii::Semaphore> vk_imageSemaphores;
    std::vector<vk::raii::Semaphore> vk_renderSemaphores;
    std::vector<vk::raii::Fence> vk_flightFences;
};

} // namespace pecs

#endif // pecs_engine_hpp