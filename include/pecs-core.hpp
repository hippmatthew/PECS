/*
 *  PECS - pecs-core.hpp
 *  Author:   Matthew Hipp
 *  Created:  1/21/24
 *  Updated:  2/20/24
 */

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
#include <optional>
#include <string>
#include <utility>
#include <vector>

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
  glm::vec3 position;

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
      .format   = vk::Format::eR32G32Sfloat,
      .offset   = offsetof(Vertex, position)
    };
  }
};

struct ProjectionData{
  alignas(16) glm::mat4 model = glm::mat4(1.0f);
  alignas(16) glm::mat4 view = glm::mat4(1.0f);
  alignas(16) glm::mat4 projection = glm::mat4(1.0f);
};

struct PhysicalProperties
{
  alignas(16) glm::vec3 position = { 0.0f, 0.0f, 0.0f };
  alignas(16) glm::vec3 momentum = { 0.0f, 0.0f, 0.0f };
  alignas(4) float energy = 0.0f;
  alignas(4) float mass = 1.0f;
  alignas(4) float charge = 1.0f;
};

struct RotationInfo
{
  float angle = 0.0f;
  glm::vec3 axis = { 0.0f, 0.0f, 1.0f };
};

struct Camera
{
  glm::mat4 view = glm::mat4(1.0f);
  glm::mat4 projection = glm::mat4(1.0f);
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

class DeviceAllocation : public Singular
{
  friend class Object;
  friend class Renderer;
  
  public:
    DeviceAllocation(const Device *, const std::vector<Vertex>&, const std::vector<unsigned int>&);

    ~DeviceAllocation();

    void allocate(std::size_t);
    void createDescriptors(std::size_t, const std::vector<vk::raii::DescriptorSetLayout>&);
    
    template <typename T>
    unsigned long addAllocation(T&, std::string);

    void updateProjection(ProjectionData&);
    void updateProperties(PhysicalProperties&);

    template <typename T>
    void updateBuffer(unsigned long, T&);

  private:
    unsigned int findMemoryIndex(unsigned int, vk::MemoryPropertyFlags);

    void createBuffers();
    
  private:
    const Device * device = nullptr;

    const unsigned long vertexSize, indexSize;
    const Vertex * vertexData;
    const unsigned int * indexData;

    vk::raii::DeviceMemory vk_modelMemory = nullptr;
    vk::raii::Buffer vk_vertexBuffer = nullptr;
    vk::raii::Buffer vk_indexBuffer = nullptr;

    vk::raii::DeviceMemory vk_projectionMemory = nullptr;
    vk::raii::Buffer vk_projectionBuffer = nullptr;
    void * projectionMapping = nullptr;

    vk::raii::DeviceMemory vk_propertiesMemory = nullptr;
    vk::raii::Buffer vk_propertiesBuffer = nullptr;
    void * propertiesMapping = nullptr;

    std::vector<vk::raii::DeviceMemory> vk_otherMemories;
    std::vector<vk::raii::Buffer> vk_otherBuffers;
    std::vector<void *> otherMappings;

    std::unordered_map<std::string, unsigned long> objectSizes;
    std::vector<std::string> objectTypes;

    vk::raii::DescriptorPool vk_descriptorPool = nullptr;
    std::vector<std::vector<vk::raii::DescriptorSet>> vk_descriptorSets;
};

class Object
{
  friend class Engine;
  friend class Renderer;

  public:
    Object(ShaderPaths);
    Object(const Object&);
    Object(Object&&);

    ~Object();

    Object& operator = (const Object&);
    Object& operator = (Object&&);
    
    Object& addEnergy(float);
    Object& addMomentum(glm::vec3);
    Object& addMass(float);
    Object& addCharge(float);
    Object& translate(glm::vec3);
    Object& rotate(RotationInfo);

    const glm::vec3& momentum() const;
    const glm::vec3& position() const;
    const float& mass() const;

    template <typename T>
    void updateCustomProperty(unsigned long, T&);

    void clean(bool removeAllocation = true);
    
  private:
    std::vector<char> readShader(std::string) const;
    std::vector<vk::raii::ShaderModule> createShaderModules(const vk::raii::Device&) const;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderCreateInfos(const std::vector<vk::raii::ShaderModule>&) const;

    void createGraphicsPipeline(const vk::raii::Device&, const ViewportInfo&);

  private:
    bool valid = false;
    bool hasTransformed = false;

    glm::mat4 nextTransformation = glm::mat4(1.0f);

    std::vector<vk::raii::DescriptorSetLayout> vk_descriptorLayouts;
    vk::raii::PipelineLayout vk_graphicsLayout = nullptr;
    vk::raii::Pipeline vk_graphicsPipeline = nullptr;

  protected:
    DeviceAllocation * allocation = nullptr;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    ShaderPaths shaderPaths;
    
    ProjectionData projection;
    PhysicalProperties properties;
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
    void createCommandPools(const vk::raii::Device&, const std::vector<unsigned int>&);
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
    
    vk::raii::DescriptorSetLayout vk_globalLayout = nullptr;
    
    vk::raii::DescriptorPool vk_globalPool = nullptr;
    vk::raii::DescriptorSet vk_globalSet = nullptr;

    Camera camera;
    std::vector<Object *> objects;

  protected:
    Settings::Engine settings;
    float deltaTime = 0.0f;
};

} // namespace pecs

#endif // pecs_core_hpp