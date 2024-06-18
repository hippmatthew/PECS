#ifndef vecs_core_renderer_hpp
#define vecs_core_renderer_hpp

#include "src/core/include/entitycontroller.hpp"
#include "src/core/include/graphics.hpp"
#include "src/core/include/synchronization.hpp"

#include <map>

#define VECS_RENDERER_SYSTEM 0x01u

namespace vecs
{

struct Camera
{
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
};

class Renderer
{
  public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;

    ~Renderer() = default;

    Renderer& operator = (const Renderer&) = delete;
    Renderer& operator = (Renderer&&) = delete;

    void load(const Device&);
    void render(const Device&, GUI&);

  private:
    unsigned int findIndex(const vk::raii::PhysicalDevice&, unsigned int, vk::MemoryPropertyFlags) const;
    void initializeSyncObjects() const;
    
    void createCommands(const Device&, unsigned long);
    void createBuffers(const Device&, const std::shared_ptr<GraphicsComponent>&);
    void allocateMemory(const Device&);
    void addPipeline(const Device&, const std::shared_ptr<GraphicsComponent>&);
    void allocatePools(const Device&);
    void allocateDescriptors(const Device&, const std::shared_ptr<GraphicsComponent>&, unsigned long);
    void loadModels(const Device&, const std::shared_ptr<GraphicsComponent>&, unsigned long);
    void updateBuffers(const Device&, const std::shared_ptr<GraphicsComponent>&, unsigned long);

  private:
    std::vector<std::shared_ptr<GraphicsComponent>> entities;
    unsigned long frameIndex = 0;
    
    vk::raii::DeviceMemory vk_stagingMemory = nullptr;
    vk::raii::DeviceMemory vk_deviceMemory = nullptr;
    std::vector<std::vector<unsigned int>> stagingOffsets;
    std::vector<unsigned int> deviceOffsets;

    vk::raii::Buffer vk_cameraTransfer = nullptr;
    vk::raii::Buffer vk_cameraDevice = nullptr;
    std::vector<std::vector<vk::raii::Buffer>> vk_transferBuffers;
    std::vector<std::vector<vk::raii::Buffer>> vk_deviceBuffers;
  
    std::map<std::string, vk::raii::PipelineLayout> pipelineLayoutMap;
    std::map<std::string, vk::raii::Pipeline> pipelineMap;

    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    std::map<std::string, std::vector<vk::raii::DescriptorSetLayout>> descriptorLayoutMap;
    std::vector<vk::raii::DescriptorSets> vk_descriptorSets;

    vk::raii::CommandPool vk_renderPool = nullptr;
    vk::raii::CommandPool vk_transferPool = nullptr;
    vk::raii::CommandBuffers vk_renderCommands = nullptr;
    vk::raii::CommandBuffers vk_transferCommands = nullptr;
};

} // namespace vecs

#endif // vecs_core_renderer_hpp