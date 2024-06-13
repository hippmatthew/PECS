#ifndef pecs_core_hpp
#define pecs_core_hpp

#ifndef pecs_include_vulkan
#define pecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // pecs_include_vulkan

#ifndef pecs_include_glfw
#define pecs_include_glfw

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // pecs_include_glfw

#ifndef pecs_include_glm
#define pecs_include_glm

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif // pecs_include_glm

#include <chrono>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#define VK_KHR_PORTABILITY_SUBSET_NAME "VK_KHR_portability_subset"

#define PECS_VALIDATION_BIT 0x1u

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

typedef std::optional<std::string> ShaderPath;
typedef std::pair<vk::Extent2D, vk::Format> ViewportInfo;
typedef std::vector<std::pair<vk::ShaderStageFlagBits, vk::raii::ShaderModule>> ShaderModules;
typedef std::vector<vk::PipelineShaderStageCreateInfo> ShaderInfos;

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

struct ShaderPaths
{
  ShaderPath vertex;
  ShaderPath fragment;
  ShaderPath compute;
};

struct Vertex
{
  alignas(16) glm::vec3 position;

  static vk::VertexInputBindingDescription bindingDescription()
  {
    return vk::VertexInputBindingDescription{
      .binding    = 0,
      .stride     = sizeof(Vertex),
      .inputRate  = vk::VertexInputRate::eVertex
    };
  }

  static vk::VertexInputAttributeDescription attributeDescription()
  {
    return vk::VertexInputAttributeDescription{
      .location = 0,
      .binding  = 0,
      .format   = vk::Format::eR32G32B32Sfloat,
      .offset   = offsetof(Vertex, position)
    };
  }
};

struct ProjectionData{
  alignas(16) glm::mat4 model = glm::mat4(1.0f);
  alignas(16) glm::mat4 view = glm::mat4(1.0f);
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
};

struct GraphicsShaders
{
  ShaderPath vertex;
  ShaderPath tesselation1;
  ShaderPath tesselation2;
  ShaderPath geometry;
  ShaderPath fragment;

  std::vector<ShaderPath> array() const
  {
    return { vertex, tesselation1, tesselation2, geometry, fragment };
  }
};

struct Camera
{
  alignas(16) glm::mat4 view = glm::mat4(1.0f);
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
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
      float fov = 90.0f;
    };

    struct GUI
    {
      std::string windowTitle = "PECS Application";
      int width = 1280;
      int height = 720;
    };

    struct Renderer
    {
      unsigned int maxFlightFrames = 2;
      vk::ClearValue backgroundColor = vk::ClearValue{ vk::ClearColorValue{ std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f } } };
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
    const vk::Image& image(const unsigned int&) const;
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
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
    vk::SurfaceFormatKHR vk_surfaceFormat;
    vk::PresentModeKHR vk_presentMode;
    vk::Extent2D vk_extent;
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

class Component : public Singular
{
  public:
    Component() = default;

    virtual ~Component() = default;

    virtual void initialize() = 0;
};

class PipelineComponent : public Component
{
  public:
    PipelineComponent() = default;

    virtual ~PipelineComponent() = default;

    virtual void initialize() {}
  
  protected:
    std::vector<char> readShader(std::string) const;
    unsigned int findMemoryIndex(const vk::raii::PhysicalDevice&, unsigned int, vk::MemoryPropertyFlags) const;

    virtual void createPipeline(const vk::raii::Device&) = 0;

  protected:
    vk::raii::PipelineLayout vk_pipelineLayout = nullptr;
    vk::raii::Pipeline vk_pipeline = nullptr; 
};

class GraphicsComponent : public PipelineComponent
{
  friend class Renderer;
  
  public:
    GraphicsComponent(const GraphicsShaders&, const std::vector<Vertex>&, const std::vector<unsigned int>&);

    ~GraphicsComponent() = default;

    GraphicsComponent& translateModel(glm::vec3);
    GraphicsComponent& rotateModel(float, glm::vec3);

    void initialize(const Device&, const ViewportInfo&, const vk::raii::Buffer&, unsigned long);

  private:
    ShaderModules createShaderModules(const vk::raii::Device&) const;
    ShaderInfos shaderInfos(const ShaderModules&) const;

    void createPipeline(const vk::raii::Device&);
    void allocateBuffers(const Device&);
    void createDescriptorSets(const vk::raii::Device&, unsigned long, const vk::raii::Buffer&);

  private:
    const GraphicsShaders paths;
    ViewportInfo i_viewport;
    alignas(16) glm::mat4 model = glm::mat4(1.0f);

    const std::vector<Vertex> vertices;
    const std::vector<unsigned int> indices;

    std::vector<vk::raii::DescriptorSetLayout> vk_descriptorLayouts;

    vk::raii::DeviceMemory vk_modelMemory = nullptr;
    vk::raii::Buffer vk_vertexBuffer = nullptr;
    vk::raii::Buffer vk_indexBuffer = nullptr;

    vk::raii::DeviceMemory vk_globalMemory = nullptr;
    vk::raii::Buffer vk_cameraBuffer = nullptr;
    void * cameraMapping = nullptr;

    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    std::vector<std::vector<vk::raii::DescriptorSet>> vk_descriptorSets;
};

class Object
{
  friend class Engine;
  friend class Renderer;

  public:
    Object() = default;
    Object(const Object&) = delete;
    Object(Object&&);

    ~Object();

    Object& operator = (const Object&) = delete;
    Object& operator = (Object&&);

  protected:
    GraphicsComponent * graphics = nullptr;
};

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

class Engine : public Singular
{
  public:
    Engine();
    Engine(const Settings&);

    virtual ~Engine();

    void run();
    void addObject(Object *);
    
    virtual void Main() {};
    
  private:   
    const ViewportInfo viewportInfo() const;

    void initialize(const Settings&);
    void createInstance();
    void createSyncObjects();
    void allocateCamera();

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

    vk::raii::DeviceMemory vk_cameraMemory = nullptr;
    vk::raii::Buffer vk_cameraBuffer = nullptr;
    void * cameraMapping = nullptr;

    Camera camera;
    std::vector<Object *> objects;

  protected:
    Settings::Engine settings;
    double pecs_deltaTime = 0.0;
    double pecs_elapsedTime = 0.0;
};

FamilyType to_family(unsigned int);

} // namespace pecs

namespace std
{

std::string to_string(pecs::FamilyType);

} // namespace std

#endif // pecs_core_hpp