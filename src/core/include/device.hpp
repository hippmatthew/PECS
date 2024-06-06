#ifndef pecs_core_device_hpp
#define pecs_core_device_hpp

#include "src/core/include/gui.hpp"

#include <optional>
#include <map>

#define PECS_GRAPHCIS_QUEUE_BIT 0x000001u
#define PECS_PRESENT_QUEUE_BIT  0x000010u
#define PECS_SYNC_COMPUTE_BIT   0x000100u
#define PECS_SYNC_TRANSFER_BIT  0x001000u
#define PECS_ASYNC_COMPUTE_BIT  0x010000u
#define PECS_ASYNC_TRANSFER_BIT 0x100000u

namespace pecs
{

typedef std::pair<std::optional<unsigned int>, vk::raii::Queue> Queue;
typedef std::pair<unsigned int, unsigned long> queueIndex;

enum QueueType
{
  Graphics,
  Present,
  SyncCompute,
  SyncTransfer,
  AsyncCompute,
  AsyncTransfer,
};

class Device : public Singular
{
  private:
    class QueueFamily : public Singular
    {
      friend class QueueFamilies;
      
      public:
        QueueFamily(unsigned int);

        ~QueueFamily();

        bool operator & (const unsigned int) const;
        QueueFamily& operator ^ (const unsigned int);
        
        const unsigned int index() const;
        const unsigned int types() const;
        const vk::raii::Queue& queue() const;

      private:
        unsigned int qf_index = 0xFFFFFFFFu;
        unsigned int qf_types = 0x0u;
        vk::raii::Queue qf_queue = nullptr;
    };
    
    class QueueFamilies : public Singular
    {
      friend class Device;
      
      public:
        QueueFamilies() = default;

        ~QueueFamilies();

        const bool isComplete() const;

        void newFamily();
        unsigned int searchFor(QueueType);
      
      private:
        
        std::vector<QueueFamily *> familes;
    };
    
  public:
    
    class Queues : public Singular
    {
      public:
        Queues(const vk::raii::PhysicalDevice&, const vk::raii::SurfaceKHR&);

        ~Queues() = default;

        unsigned int index(QueueType) const;
        const vk::raii::Queue& queue(QueueType) const;
        std::vector<unsigned int> array() const;
        bool isComplete() const;

        void setQueues(const vk::raii::Device&);

      private:
        Queue graphics = Queue(std::nullopt, nullptr);
        Queue present = Queue(std::nullopt, nullptr);
        Queue compute = Queue(std::nullopt, nullptr);
        Queue transfer = Queue(std::nullopt, nullptr);
    };
  
  public:
    Device(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);

    ~Device();

    const vk::raii::PhysicalDevice& physical() const;
    const vk::raii::Device& logical() const;
    const std::vector<unsigned int> queueFamilyArray() const;
    const vk::raii::Queue& queue(QueueType) const;

  private:
    bool supportsExtensions(const vk::raii::PhysicalDevice&) const;
    
    vk::raii::PhysicalDevice bestPhysicalDevice(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);
    void createLogicalDevice();

  private:
    std::vector<const char *> extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
    
    Queues * queues = nullptr;
    
    vk::raii::PhysicalDevice vk_physicalDevice = nullptr;
    vk::raii::Device vk_device = nullptr;
};

} // namespace pecs

#endif // pecs_core_device_hpp