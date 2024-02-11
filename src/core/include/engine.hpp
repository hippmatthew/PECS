/*
 *  PECS::core - engine.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/8/24
 */

#ifndef pecs_core_engine_hpp
#define pecs_core_engine_hpp

#include "src/core/include/renderer.hpp"

namespace pecs
{

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    ~Engine();

    void run();
    void addObject(Object&);
    
    virtual void Main() {};
    
  private:   
    const ViewportInfo viewportInfo() const;

    void initialize(const Settings&);
    void createInstance();
    void createSyncObjects();

  private:
    std::vector<const char *> layers;
    unsigned int frameIndex = 0;
    
    GUI * gui = nullptr;
    Device * device = nullptr;
    Renderer * renderer = nullptr;
    
    vk::raii::Instance vk_instance = nullptr;

    std::vector<vk::raii::Semaphore> vk_imageSemaphores;
    std::vector<vk::raii::Semaphore> vk_renderSemaphores;
    std::vector<vk::raii::Fence> vk_flightFences;

  protected:
    Settings::Engine settings;
    std::vector<Object *> objects;
};

} // namespace pecs

#endif // pecs_core_engine_hpp