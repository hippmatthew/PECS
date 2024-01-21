/*
 *  PECS - pecs.hpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  1/21/24
 */

#ifndef pecs_hpp
#define pecs_hpp

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

#include <string>
#include <vector>

#define PECS_VALIDATION_BIT 0x1u

namespace pecs
{

typedef std::pair<std::optional<unsigned int>, vk::raii::Queue> Queue;
typedef std::optional<std::string> ShaderPath;

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
      int width = 1280;
      int height = 720;
    };

  public:
    Settings() = default;
    Settings(const Settings&);
    Settings(Settings&&) = delete;

    ~Settings() = default;

    Settings& operator = (const Settings&);
    Settings& operator = (Settings&&) = delete;

  public:
    Settings::Engine engine;
    Settings::GUI gui;
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

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vk::raii::PhysicalDevice&, const vk::raii::Device&);

  private:
    void initialize();
    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&);
    void choosePresentMode(const vk::raii::PhysicalDevice&);
    void chooseExtent(const vk::raii::PhysicalDevice&);
    void createImageViews(const vk::raii::Device&);
  
  private:
    Settings::GUI settings;

    GLFWwindow * gl_window;

    vk::raii::SurfaceKHR vk_surface = nullptr;
    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
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

  private:
    bool supportsExtensions(const vk::raii::PhysicalDevice&) const;
    
    vk::raii::PhysicalDevice bestPhysicalDevice(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);
    void createLogicalDevice();

  private:
    std::vector<const char *> extensions;
    
    Queues * queues = nullptr;
    
    vk::raii::PhysicalDevice vk_physicalDevice = nullptr;
    vk::raii::Device vk_device = nullptr;
};

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    ~Engine();

    void run();

  private:
    void initialize();
    void createInstance();

  private:
    Settings::Engine settings;
    std::vector<const char *> layers;
    
    GUI * gui = nullptr;
    Device * device = nullptr;

    vk::raii::Instance vk_instance = nullptr;
};

class Pipeline : public Singular
{
  public:
    Pipeline(const ShaderPaths& s) : shaderPaths(s) {}

    virtual ~Pipeline() = default;
  
  protected:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStageCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

  protected:
    ShaderPaths shaderPaths;

    vk::raii::Pipeline vk_pipeline = nullptr;
    vk::raii::PipelineLayout vk_layout = nullptr;
};

class GraphicsPipeline : public Pipeline
{
  public:
    GraphicsPipeline(const vk::raii::Device&, const vk::Extent2D&, const vk::Format&, const ShaderPaths&);

    ~GraphicsPipeline() = default;
};

} // namespace pecs

#endif // pecs_hpp