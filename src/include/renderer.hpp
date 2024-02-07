/*
 *  PECS - renderer.hpp 
 *  Author:   Matthew Hipp
 *  Created:  2/4/24
 *  Updated:  2/7/24
 */

#ifndef pecs_renderer_hpp
#define pecs_renderer_hpp

#include "src/include/object.hpp"

namespace pecs
{

class Renderer : public Singular
{
  public:
    Renderer(const Settings::Renderer&, const Device&, const ViewportInfo&);

    ~Renderer() = default;

    const std::vector<vk::raii::CommandBuffer>& commandBuffers() const;
    unsigned int maxFlightFrames() const;
    
    void start(const unsigned int&, const vk::raii::Image&, const vk::raii::ImageView&);
    void render(const Object&, const unsigned int&, const vk::raii::Image&, const vk::raii::ImageView&);
    void stop(const unsigned int&, const vk::raii::Image&);

  private:
    void createCommandPool(const vk::raii::Device&, const std::vector<unsigned int>&);
    void createCommandBuffers(const vk::raii::Device&);

  private:
    Settings::Renderer settings;
    ViewportInfo i_viewport;
    vk::Rect2D renderArea;
    vk::ClearValue vk_clearValue = vk::ClearValue{ vk::ClearColorValue{std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f } } };
    
    vk::raii::CommandPool vk_commandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> vk_commandBuffers;
};

} // namespace pecs

#endif // pecs_renderer_hpp