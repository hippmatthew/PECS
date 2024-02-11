/*
 *  PECS::core - renderer.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/4/24
 *  Updated:  2/9/24
 */

#ifndef pecs_core_renderer_hpp
#define pecs_core_renderer_hpp

#include "src/core/include/object.hpp"

namespace pecs
{

class Renderer : public Singular
{
  public:
    Renderer(const Settings::Renderer&, const Device&, const ViewportInfo&);

    ~Renderer() = default;

    const std::vector<vk::raii::CommandBuffer>& commandBuffers() const;
    unsigned int maxFlightFrames() const;
    
    void createObjectBuffers(std::vector<Object *>&, const Device&);
    void render(std::vector<Object *>&, const unsigned int&, const vk::raii::Image&, const vk::raii::ImageView&, const Device&);

  private:
    unsigned int findMemoryIndex(const vk::raii::PhysicalDevice&, unsigned int, vk::MemoryPropertyFlags) const;
    
    void createCommandPools(const vk::raii::Device&, const std::vector<unsigned int>&);
    void createRenderBuffers(const vk::raii::Device&);
    void beginRendering(const unsigned int&, const vk::raii::Image&, const vk::raii::ImageView&);
    void endRendering(const unsigned int&, const vk::raii::Image&);
    
  private:
    Settings::Renderer settings;
    ViewportInfo i_viewport;
    vk::Rect2D renderArea;
    vk::ClearValue vk_clearValue = vk::ClearValue{ vk::ClearColorValue{std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f } } };
    
    vk::raii::CommandPool vk_renderPool = nullptr;
    vk::raii::CommandPool vk_transferPool = nullptr;
    std::vector<vk::raii::CommandBuffer> vk_renderBuffers;

    vk::raii::DeviceMemory vk_objectMemory = nullptr;
    vk::raii::Buffer vk_vertexBuffer = nullptr;
    vk::raii::Buffer vk_indexBuffer = nullptr;
};

} // namespace pecs

#endif // pecs_core_renderer_hpp