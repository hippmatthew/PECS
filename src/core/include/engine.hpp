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

struct Camera
{
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
};

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
    
    vk::raii::DescriptorSetLayout vk_globalLayout = nullptr;
    
    vk::raii::DescriptorPool vk_globalPool = nullptr;
    vk::raii::DescriptorSet vk_globalSet = nullptr;

    Camera camera;
    std::vector<Object *> objects;

  protected:
    Settings::Engine settings;
    float deltaTime = 0.0f;
};

} // namespace pecs

#endif // pecs_core_engine_hpp