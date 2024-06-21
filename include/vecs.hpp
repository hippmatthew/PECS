// vecs_hpp version 0.0.6.5 generated on 06-21-2024 16:55:45

#ifndef vecs_hpp
#define vecs_hpp

#ifndef vecs_include_dependencies
#define vecs_include_dependencies

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif // vecs_include_dependencies

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vecs
{

class Settings;
class Material;
class GUI;
class Device;
class Synchronization;
class Engine;

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

enum ShaderType
{
  Vertex,
  Tesselation1,
  Tesselation2,
  Geometry,
  Fragment,
  ComputeShader
};

class Settings
{ 
  public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;
    
    Settings& operator = (const Settings&) = delete;
    Settings& operator = (Settings&&) = delete;
    
    static Settings& instance();
    static void destroy();

    std::string name() const;
    unsigned int version() const;
    bool validation_enabled() const;
    std::string title() const;
    unsigned int width() const;
    unsigned int height() const;
    bool portability_enabled() const;
    std::vector<const char *> device_extensions() const;
    vk::Format format() const;
    vk::ColorSpaceKHR color_space() const;
    vk::PresentModeKHR present_mode() const;
    vk::Extent2D extent() const;
    unsigned long max_flight_frames() const;
    vk::ClearValue background_color() const;

    Settings& update_name(std::string);
    Settings& update_version(unsigned int);
    Settings& toggle_validation();
    Settings& update_title(std::string);
    Settings& update_width(unsigned int);
    Settings& update_height(unsigned int);
    Settings& toggle_portability();
    Settings& add_device_extension(const char *);
    Settings& remove_device_extension(const char *);
    Settings& update_format(vk::Format);
    Settings& update_color_space(vk::ColorSpaceKHR);
    Settings& update_present_mode(vk::PresentModeKHR);
    Settings& update_extent(unsigned int width, unsigned int height);
    Settings& update_max_flight_frames(unsigned long);
    Settings& update_background_color(vk::ClearValue);
    
    void set_default();

  private:
    Settings() = default;
    ~Settings() = default;
  
  private:
    static Settings * p_settings;
    
    std::string s_name = "VECS Application";
    unsigned int s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    bool s_validationEnabled = true;
    
    std::string s_title = "VECS Application";
    unsigned int s_width = 1280;
    unsigned int s_height = 720;

    bool s_portabilityEnabled = false;

    std::vector<const char *> s_gpuExtensions{
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    vk::Format s_format = vk::Format::eB8G8R8A8Srgb;
    vk::ColorSpaceKHR s_colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR s_presentMode = vk::PresentModeKHR::eMailbox;
    vk::Extent2D s_extent{
      .width = s_width,
      .height = s_height
    };

    unsigned long s_maxFrames = 2;
    vk::ClearValue s_backColor = vk::ClearValue{vk::ClearColorValue{std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f }}};
};


class Material
{ 
  public:
    class MaterialBuilder
    {
      public:
        MaterialBuilder(std::string);
        MaterialBuilder(const MaterialBuilder&) = delete;
        MaterialBuilder(MaterialBuilder&&) = delete;

        ~MaterialBuilder();

        MaterialBuilder& operator = (const MaterialBuilder&) = delete;
        MaterialBuilder& operator = (MaterialBuilder&&) = delete;

        MaterialBuilder& vertex_path(std::string);
        MaterialBuilder& tesselation1_path(std::string);
        MaterialBuilder& tesselation2_path(std::string);
        MaterialBuilder& geometry_path(std::string);
        MaterialBuilder& fragment_path(std::string);
        MaterialBuilder& compute_path(std::string);
        Material& build() const;

      private:
        Material * material = nullptr;
    };

  public:
    Material() = default;
    Material(const Material&) = default;
    Material(Material&&) = default;

    ~Material() = default;

    Material& operator = (const Material&) = default;
    Material& operator = (Material&&) = default;

    static MaterialBuilder Builder(std::string);

  private:
    std::vector<ShaderType> shaders() const;
    std::vector<char> binary(ShaderType) const;
  
  private:
    std::string tag;
    std::optional<std::string> vertex;
    std::optional<std::string> tesselation1;
    std::optional<std::string> tesselation2;
    std::optional<std::string> geometry;
    std::optional<std::string> fragment;
    std::optional<std::string> compute;

  friend class MaterialBuilder;
};

class GUI
{ 
  public:
    GUI();
    GUI(const GUI&) = delete;
    GUI(GUI&&) = delete;

    ~GUI();

    GUI& operator = (const GUI&) = delete;
    GUI& operator = (GUI&&) = delete;

    std::vector<const char *> extensions() const;
    bool shouldClose() const;
    void pollEvents() const;
    const vk::raii::SurfaceKHR& surface() const;
    const vk::raii::SwapchainKHR& swapchain() const;
    const vk::Image& image(unsigned long) const;
    const vk::raii::ImageView& imageView(unsigned long) const;

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vecs::Device&);
    void recreateSwapchain(const vecs::Device&);
  
  private:
    static void resizeFramebuffer(GLFWwindow *, int, int);

    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&) const;
    void choosePresentMode(const vk::raii::PhysicalDevice&) const;
    void chooseExtent(const vk::raii::PhysicalDevice&) const;

    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void createImageViews(const vk::raii::Device&);

  private:
    GLFWwindow * gl_window = nullptr;
    bool modifiedFramebuffer = false;

    vk::raii::SurfaceKHR vk_surface = nullptr;
    
    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
};

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

class Synchronization
{
  public:
    Synchronization(const Synchronization&) = delete;
    Synchronization(Synchronization&&) = delete;

    Synchronization& operator = (const Synchronization&) = delete;
    Synchronization& operator = (Synchronization&&) = delete;
    
    static Synchronization& instance();
    static void destroy();

    const vk::raii::Fence& fence(std::string) const;
    const vk::raii::Semaphore& semaphore(std::string) const;
    void wait_fences(std::vector<std::string>) const;

    void add_fence(std::string, bool signaled = false);
    void add_semaphore(std::string);
    void remove_fence(std::string);
    void remove_semaphore(std::string);

    void link_device(std::shared_ptr<Device>);
  
  private:
    Synchronization() = default;
    ~Synchronization() = default;

  private:
    static Synchronization * p_sync;
    
    std::shared_ptr<Device> p_device = nullptr;
    
    std::map<std::string, vk::raii::Fence> fenceMap;
    std::map<std::string, vk::raii::Semaphore> semaphoreMap;
};


class Engine
{
  public:
    Engine() = default;
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

    ~Engine();

    Engine& operator = (const Engine&) = delete;
    Engine& operator = (Engine&&) = delete;

    void initialize();
    
    virtual void run();

  private:
    void createInstance();

  private:
    GUI * gui = nullptr;
    std::shared_ptr<Device> device = nullptr;
    
    vk::raii::Instance vk_instance = nullptr;
};

FamilyType to_family(unsigned int);
unsigned int to_bits(FamilyType);
} // namespace vecs

namespace std
{

std::string to_string(vecs::FamilyType);

} // namspace std

#endif // vecs_hpp
