/*
 *  PECS::core - engine.hpp 
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/20/24
 */

#ifndef pecs_core_engine_hpp
#define pecs_core_engine_hpp

#include "src/core/include/renderer.hpp"

#include <chrono>

namespace pecs
{

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    virtual ~Engine();

    void run();
    void addObject(Object *);
    
    virtual void Main() {};
    
  private:   
    const ViewportInfo viewportInfo() const;

    void initialize(const Settings&);
    void createInstance();
    void createSyncObjects();
    void allocateCamera();

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

    vk::raii::DeviceMemory vk_cameraMemory = nullptr;
    vk::raii::Buffer vk_cameraBuffer = nullptr;
    void * cameraMapping = nullptr;

    Camera camera;
    std::vector<Object *> objects;

  protected:
    Settings::Engine settings;
    double pecs_deltaTime = 0.0;
    double pecs_elapsedTime = 0.0;
};

} // namespace pecs

#endif // pecs_core_engine_hpp