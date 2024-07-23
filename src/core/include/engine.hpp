#ifndef vecs_core_engine_hpp
#define vecs_core_engine_hpp

#include "src/core/include/entities.hpp"
#include "src/core/include/components.hpp"
#include "src/core/include/systems.hpp"
#include "src/core/include/device.hpp"

namespace vecs
{

class Engine
{
  public:
    Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

    ~Engine();

    Engine& operator = (const Engine&) = delete;
    Engine& operator = (Engine&&) = delete;
    
    virtual void load();
    virtual void run();
  
  private:
    void createInstance();

  protected:
    virtual bool close_condition();
    
    bool should_close() const;
    const Device& device() const;
    
    void initialize(void * p_next = nullptr);
    void poll_gui();
  
  private:
    std::unique_ptr<GUI> vecs_gui = nullptr;
    std::shared_ptr<Device> vecs_device = nullptr;

    vk::raii::Instance vk_instance = nullptr;

  protected:
    std::unique_ptr<EntityManager> entity_manager = nullptr;
    std::shared_ptr<ComponentManager> component_manager = nullptr;
    std::unique_ptr<SystemManager> system_manager = nullptr;
};

} // namespace vecs

#endif // vecs_core_engine_hpp