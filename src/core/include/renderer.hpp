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
    
    void render(const std::vector<Object *>&, const unsigned int&, const vk::Image&, const vk::raii::ImageView&, const Device&);

  private:
    void createCommandPools(const vk::raii::Device&, const unsigned long&);
    void createRenderBuffers(const vk::raii::Device&);
    void beginRendering(const unsigned int&, const vk::Image&, const vk::raii::ImageView&);
    void endRendering(const unsigned int&, const vk::Image&);
    
  private:
    Settings::Renderer settings;
    ViewportInfo i_viewport;
    
    vk::raii::CommandPool vk_renderPool = nullptr;
    std::vector<vk::raii::CommandBuffer> vk_renderBuffers;
};

} // namespace pecs

#endif // pecs_core_renderer_hpp