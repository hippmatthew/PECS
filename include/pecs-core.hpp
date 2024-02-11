/*
 *  PECS - pecs-core.hpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/9/24
 */

#ifndef pecs_core_hpp
#define pecs_core_hpp

#ifndef pecs_include_vulkan
#define pecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_to_string.hpp>

#endif // pecs_include_vulkan

#ifndef pecs_include_glfw
#define pecs_include_glfw

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // pecs_include_glfw

#ifndef pecs_include_glm
#define pecs_include_glm

#include <glm/glm.hpp>

#endif // pecs_include_glm

#include <string>
#include <vector>
#include <fstream>

#define PECS_VALIDATION_BIT 0x1u

namespace pecs
{

typedef std::pair<std::optional<unsigned int>, vk::raii::Queue> Queue;
typedef std::optional<std::string> ShaderPath;
typedef std::pair<vk::Extent2D, vk::Format> ViewportInfo;

enum QueueType
{
  Graphics,
  Present,
  Compute,
  Transfer
};

struct ShaderPaths
{
  ShaderPath vertex;
  ShaderPath fragment;
  ShaderPath compute;
};

struct Vertex
{
  glm::vec2 position;
  glm::vec3 color;

  static vk::VertexInputBindingDescription bindingDescription()
  {
    return vk::VertexInputBindingDescription{
      .binding    = 0,
      .stride     = sizeof(Vertex),
      .inputRate  = vk::VertexInputRate::eVertex
    };
  }

  static std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions()
  {
    return std::array<vk::VertexInputAttributeDescription, 2>{
      vk::VertexInputAttributeDescription{
        .location = 0,
        .binding  = 0,
        .format   = vk::Format::eR32G32Sfloat,
        .offset   = offsetof(Vertex, position)
      },
      vk::VertexInputAttributeDescription{
        .location = 1,
        .binding  = 0,
        .format   = vk::Format::eR32G32Sfloat,
        .offset   = offsetof(Vertex, color)
      }
    };
  }
};

class Singular
{
  public:
    Singular() = default;
    Singular(const Singular&) = delete;
    Singular(Singular&&) = delete;

    virtual ~Singular() = default;

    Singular& operator = (const Singular&) = delete;
    Singular& operator = (Singular&&) = delete;
};

class Settings
{
  public:
    struct Engine
    {
      std::string applicationName = "PECS Application";
      unsigned int applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
      unsigned int layerBits = PECS_VALIDATION_BIT;
    };

    struct GUI
    {
      std::string windowTitle = "PECS Application";
      int width = 800;
      int height = 800;
    };

    struct Renderer
    {
      unsigned int maxFlightFrames = 2;
    };

  public:
    Settings() = default;
    Settings(const Settings&) = default;
    Settings(Settings&&) = default;

    ~Settings() = default;

    Settings& operator = (const Settings&) = default;
    Settings& operator = (Settings&&) = default;

  public:
    Settings::Engine engine;
    Settings::GUI gui;
    Settings::Renderer renderer;
};

class Object : public Singular
{
  friend class Engine;
  friend class Renderer;

  public:
    Object(std::vector<Vertex>, std::vector<unsigned int>, ShaderPaths, glm::vec2 p = {0.0f, 0.0f});
    Object(ShaderPaths, glm::vec2 p = {0.0f, 0.0f});

    ~Object() = default;
    
  protected:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

    void createGraphicsPipeline(const vk::raii::Device&, const ViewportInfo&);

  protected:
    glm::vec2 position;
    ShaderPaths shaderPaths;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    vk::raii::PipelineLayout vk_graphicsLayout = nullptr;
    vk::raii::Pipeline vk_graphicsPipeline = nullptr;
};

class GUI : public Singular
{
  public:
    GUI();
    GUI(const Settings::GUI&);

    ~GUI();

    std::vector<const char *> extensions() const;
    GLFWwindow * window() const;
    const vk::raii::SurfaceKHR& surface() const;
    const vk::Extent2D& extent() const;
    const vk::Format& format() const;
    const vk::raii::SwapchainKHR& swapchain() const;
    const vk::raii::Image& image(const unsigned int&) const;
    const vk::raii::ImageView& imageView(const unsigned int&) const;

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void recreateSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);

  private:
    void initialize();
    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&);
    void choosePresentMode(const vk::raii::PhysicalDevice&);
    void chooseExtent(const vk::raii::PhysicalDevice&);
    void createImageViews(const vk::raii::Device&);
    static void resizeFramebuffer(GLFWwindow *, int, int);
  
  private:
    Settings::GUI settings;
    bool framebufferChanged = false;

    GLFWwindow * gl_window;

    vk::raii::SurfaceKHR vk_surface = nullptr;
    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::raii::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
    vk::SurfaceFormatKHR vk_surfaceFormat;
    vk::PresentModeKHR vk_presentMode;
    vk::Extent2D vk_extent;
};

class Device : public Singular
{
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
    std::vector<vk::raii::CommandBuffer> vk_transferBuffers;

    vk::raii::DeviceMemory vk_vertexMemory = nullptr;
    std::vector<vk::raii::Buffer> vk_vertexBuffers;
};

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    ~Engine();

    void run();
    void addObject(Object&);
    
    virtual void Main() {};
    
  private:   
    const ViewportInfo viewportInfo() const;

    void initialize(const Settings&);
    void createInstance();
    void createSyncObjects();

  private:
    std::vector<const char *> layers;
    unsigned int frameIndex = 0;
    
    GUI * gui = nullptr;
    Device * device = nullptr;
    Renderer * renderer = nullptr;
    
    vk::raii::Instance vk_instance = nullptr;

    std::vector<vk::raii::Semaphore> vk_imageSemaphores;
    std::vector<vk::raii::Semaphore> vk_renderSemaphores;
    std::vector<vk::raii::Fence> vk_flightFences;

  protected:
    Settings::Engine settings;
    std::vector<Object *> objects;
};

} // namespace pecs

#endif // pecs_core_hpp