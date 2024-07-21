#ifndef vecs_core_device_hpp
#define vecs_core_device_hpp

#include "src/core/include/extras.hpp"
#include "src/core/include/gui.hpp"

#ifndef vecs_include_vulkan
#define vecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // vecs_include_vulkan

#include <map>
#include <memory>
#include <vector>

namespace vecs
{

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

        ~QueueFamilies() = default;

        QueueFamilies& operator = (const QueueFamilies&) = delete;
        QueueFamilies& operator = (QueueFamilies&&) = delete;

      private:
        void addFamily(unsigned long, FamilyType);
        void setQueues(const vk::raii::Device&);

      private:
        std::map<std::string, std::shared_ptr<QueueFamily>> familyMap;
        std::vector<FamilyType> supportedFamilies;
    };
  
  public:
    Device(const vk::raii::Instance&, const vecs::GUI&, const void * p_next = nullptr);
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    ~Device() = default;

    Device& operator = (const Device&) = delete;
    Device& operator = (Device&&) = delete;

    const vk::raii::PhysicalDevice& physical() const;
    const vk::raii::Device& logical() const;
    bool hasFamily(FamilyType) const;
    unsigned long familyIndex(FamilyType) const;
    const vk::raii::Queue& queue(FamilyType) const;

  private:
    void getGPU(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);
    void createDevice(const void *);

  private:
    std::unique_ptr<QueueFamilies> queueFamilies = nullptr;

    vk::raii::PhysicalDevice vk_physicalDevice = nullptr;
    vk::raii::Device vk_device = nullptr;
};

FamilyType to_family(unsigned int);
unsigned int to_bits(FamilyType);

} // namespace vecs

namespace std
{

std::string to_string(vecs::FamilyType);

} // namespace std

#endif // vecs_core_device_hpp