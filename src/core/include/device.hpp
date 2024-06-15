#ifndef vecs_core_device_hpp
#define vecs_core_device_hpp

#include "src/core/include/gui.hpp"

#ifndef vecs_include_vulkan
#define vecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // vecs_include_vulkan

#include <map>
#include <vector>

#define VECS_GRAPHICS_QUEUE_BIT       0x1000000u
#define VECS_PRESENT_QUEUE_BIT        0x0100000u
#define VECS_SYNC_COMPUTE_QUEUE_BIT   0x0010000u
#define VECS_ASYNC_COMPUTE_QUEUE_BIT  0x0001000u
#define VECS_SYNC_TRANSFER_QUEUE_BIT  0x0000100u
#define VECS_ASYNC_TRANSFER_QUEUE_BIT 0x0000010u
#define VECS_SPARSE_QUEUE_BIT         0x0000001u

#define VECS_ALL_FAMILY       ( VECS_GRAPHICS_QUEUE_BIT | VECS_PRESENT_QUEUE_BIT | VECS_SYNC_COMPUTE_QUEUE_BIT | VECS_SYNC_TRANSFER_QUEUE_BIT )
#define VECS_COMPUTE_FAMILY   ( VECS_ASYNC_COMPUTE_QUEUE_BIT )
#define VECS_TRANSFER_FAMILY  ( VECS_ASYNC_TRANSFER_QUEUE_BIT )
#define VECS_ASYNC_FAMILY     ( VECS_ASYNC_COMPUTE_QUEUE_BIT | VECS_ASYNC_TRANSFER_QUEUE_BIT )
#define VECS_SPARSE_FAMILY    ( VECS_SPARSE_QUEUE_BIT )

namespace vecs
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
  All,
  Compute,
  Transfer,
  Async,
  Sparse
};

class GUI;

class Device
{
  private:
    class QueueFamily
    {
      friend class Device;
      friend class QueueFamilies;

      public:
        QueueFamily(unsigned long, unsigned int);
        QueueFamily(const QueueFamily&) = delete;
        QueueFamily(QueueFamily&&) = delete;

        ~QueueFamily() = default;

        QueueFamily& operator = (const QueueFamily&) = delete;
        QueueFamily& operator = (QueueFamily&&) = delete;

      private:
        const unsigned long qf_index;
        const unsigned int qf_types;
        vk::raii::Queue qf_queue = nullptr;
    };

    class QueueFamilies
    {
      friend class Device;

      public:
        QueueFamilies(const vk::raii::PhysicalDevice&, const vk::raii::SurfaceKHR&);
        QueueFamilies(const QueueFamilies&) = delete;
        QueueFamilies(QueueFamilies&&) = delete;

        ~QueueFamilies();

        QueueFamilies& operator = (const QueueFamilies&) = delete;
        QueueFamilies& operator = (QueueFamilies&&) = delete;

      private:
        void addFamily(unsigned long, FamilyType);
        void setQueues(const vk::raii::Device&);

      private:
        std::map<std::string, QueueFamily *> familyMap;
        std::vector<FamilyType> supportedFamilies;
    };
  
  public:
    Device(const vk::raii::Instance&, const vecs::GUI&);
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    ~Device();

    Device& operator = (const Device&) = delete;
    Device& operator = (Device&&) = delete;

    const vk::raii::PhysicalDevice& physical() const;
    const vk::raii::Device& logical() const;
    bool hasFamily(FamilyType) const;
    unsigned long familyIndex(FamilyType) const;
    const vk::raii::Queue& queue(FamilyType) const;

  private:
    void getGPU(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);
    void createDevice();

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
unsigned int to_bits(FamilyType);

} // namespace vecs

namespace std
{

std::string to_string(vecs::FamilyType);

} //namespace std

#endif // vecs_core_device_hpp