#ifndef vecs_core_engine_hpp
#define vecs_core_engine_hpp

#include "src/core/include/renderer.hpp"

namespace vecs
{

class Engine
{
  public:
    Engine() = default;
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

    ~Engine();

    Engine& operator = (const Engine&) = delete;
    Engine& operator = (Engine&&) = delete;

    void initialize();
    
    virtual void run();

  private:
    void createInstance();

  private:
    GUI * gui = nullptr;
    std::shared_ptr<Device> device = nullptr;
    Renderer * renderer = nullptr;
    
    vk::raii::Instance vk_instance = nullptr;
};

} // namespace vecs

#endif // vecs_core_engine_hpp