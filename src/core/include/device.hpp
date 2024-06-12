#ifndef pecs_core_device_hpp
#define pecs_core_device_hpp

#include "src/core/include/gui.hpp"

#include <optional>
#include <map>

#define VK_KHR_PORTABILITY_SUBSET_NAME "VK_KHR_portability_subset"

#define PECS_GRAPHICS_QUEUE_BIT 0x0000001u
#define PECS_PRESENT_QUEUE_BIT  0x0000010u
#define PECS_SYNC_COMPUTE_BIT   0x0000100u
#define PECS_SYNC_TRANSFER_BIT  0x0001000u
#define PECS_ASYNC_COMPUTE_BIT  0x0010000u
#define PECS_ASYNC_TRANSFER_BIT 0x0100000u
#define PECS_SPARSE_BIT         0x1000000u

#define PECS_GRAPHICS_FAMILY        PECS_GRAPHICS_QUEUE_BIT | PECS_PRESENT_QUEUE_BIT
#define PECS_ASYNC_COMPUTE_FAMILY   PECS_ASYNC_COMPUTE_BIT
#define PECS_ASYNC_TRANSFER_FAMILY  PECS_ASYNC_TRANSFER_BIT
#define PECS_ASYNC_COMBINE_FAMILY   PECS_ASYNC_COMPUTE_BIT | PECS_ASYNC_TRANSFER_BIT
#define PECS_SPARSE_FAMILY          PECS_SPARSE_BIT
#define PECS_ALLQUEUE_FAMILY        PECS_GRAPHICS_FAMILY | PECS_SYNC_COMPUTE_BIT | PECS_SYNC_TRANSFER_BIT

namespace pecs
{

enum QueueType
{
  Graphics,
  Present,
  SyncCompute,
  SyncTransfer,
  AsyncCompute,
  AsyncTransfer,
  SparseBinding
};

enum FamilyType
{
  AllQueue,
  Transfer,
  Compute,
  AsyncCombine,
  Sparse,
  NoFamily
};

class Device : public Singular
{
  private:
    class QueueFamilies;
    
    class QueueFamily : public Singular
    {
      friend class Device;
      friend class QueueFamilies;
      
      public:
        QueueFamily(unsigned long, unsigned int);

        ~QueueFamily() = default;
      
      private:
        const unsigned long qf_index = -1;
        const unsigned int qf_types = 0x0000000u;
        vk::raii::Queue qf_queue = nullptr;
    };
    
    class QueueFamilies : public Singular
    { 
      friend class Device;
      
      public:
        QueueFamilies(const vk::raii::PhysicalDevice&, const vk::raii::SurfaceKHR&);

        ~QueueFamilies();
        
        void setQueues(const vk::raii::Device&);
      
      private:
        void addFamily(FamilyType, unsigned long);

      private:
        std::map<std::string, QueueFamily *> familyMap;
        std::vector<FamilyType> familyTypes;
    };
  
  public:
    Device(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);

    ~Device();

    const vk::raii::PhysicalDevice& physical() const;
    const vk::raii::Device& logical() const;
    bool hasFamily(FamilyType) const;
    const unsigned long familyIndex(FamilyType) const;
    const vk::raii::Queue& queue(FamilyType) const;

  private:
    bool supportsExtensions(const vk::raii::PhysicalDevice&) const;
    
    vk::raii::PhysicalDevice bestPhysicalDevice(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);
    void createLogicalDevice();

  private:
    std::vector<const char *> extensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };
    
    QueueFamilies * queueFamilies = nullptr;
    
    vk::raii::PhysicalDevice vk_physicalDevice = nullptr;
    vk::raii::Device vk_device = nullptr;
};

FamilyType to_family(unsigned int);

} // namespace pecs

namespace std
{

std::string to_string(pecs::FamilyType);

} // namespace std

#endif // pecs_core_device_hpp