#ifndef vecs_core_synchronization_hpp
#define vecs_core_synchronization_hpp

#include "src/core/include/device.hpp"

namespace vecs
{

class Synchronization
{
  public:
    Synchronization(const Synchronization&) = delete;
    Synchronization(Synchronization&&) = delete;

    ~Synchronization();

    Synchronization& operator = (const Synchronization&) = delete;
    Synchronization& operator = (Synchronization&&) = delete;
    
    static Synchronization& instance();
    static void destroy();

    const vk::raii::Fence& fence(std::string) const;
    const vk::raii::Semaphore& semaphore(std::string) const;
    void wait_fences(std::vector<std::string>) const;

    void add_fence(std::string, bool signaled = false);
    void add_semaphore(std::string);
    void remove_fence(std::string);
    void remove_semaphore(std::string);

    void link_device(std::shared_ptr<Device>);
  
  private:
    Synchronization() = default;

  private:
    static Synchronization * p_sync;
    
    std::shared_ptr<Device> vecs_device = nullptr;
    
    std::map<std::string, vk::raii::Fence> fenceMap;
    std::map<std::string, vk::raii::Semaphore> semaphoreMap;
};

}


#endif // vecs_core_synchronization_hpp