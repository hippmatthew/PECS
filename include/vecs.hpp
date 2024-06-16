#ifndef vecs_hpp
#define vecs_hpp

#ifndef vecs_include_vulkan
#define vecs_include_vulkan

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#endif // vecs_include_vulkan

#ifndef vecs_include_glfw
#define vecs_include_glfw

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // vecs_include_glfw

#ifndef vecs_include_glm
#define vecs_include_glm

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif // vecs_include_glm

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace vecs
{

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

enum Shader
{
  Vertex,
  Tesselation1,
  Tesselation2,
  Geometry,
  Fragment,
  Compute
};

struct VertexData
{
  static vk::VertexInputBindingDescription binding()
  {
    return vk::VertexInputBindingDescription{
      .binding  = 0,
      .stride     = sizeof(VertexData),
      .inputRate  = vk::VertexInputRate::eVertex
    };
  }

  static vk::VertexInputAttributeDescription attribute()
  {
    return vk::VertexInputAttributeDescription{
      .location = 0,
      .binding  = 0,
      .format   = vk::Format::eR32G32B32A32Sfloat,
      .offset   = __offsetof(VertexData, v_position)
    };
  }

  glm::vec3 v_position;
};

class Settings
{ 
  public:
    Settings(const Settings&) = delete;
    Settings(Settings&&) = delete;

    ~Settings();
    
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
    
    void set_default();

  private:
    Settings() = default;
  
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
};

class Entity
{
  public:
    Entity() = default;
    Entity(unsigned int);
    Entity(const Entity&) = default;
    Entity(Entity&&) = default;

    ~Entity() = default;

    Entity& operator = (const Entity&) = default;
    Entity& operator = (Entity&&) = default;

    unsigned int id() const;

    Entity& addSystems(unsigned int);

  private:
    unsigned int e_id = 0x0u;
};

class EntityController
{
  public:
    EntityController(const EntityController&) = delete;
    EntityController(EntityController&&) = delete;

    ~EntityController();

    EntityController& operator = (const EntityController&) = delete;
    EntityController& operator = (EntityController&&) = delete;

    static EntityController& instance();
    static void destroy();

    const std::list<Entity *>& entities() const;

    void add_entities(std::list<Entity *>&);

  private:
    EntityController() = default;
  
  private:
    static EntityController * p_controller;
    std::list<Entity *> e_list;
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
    std::vector<Shader> shaders() const;
    std::vector<char> binary(Shader) const;
  
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

class Custom
{
  public:
    Custom() = default;
    Custom(const Custom&) = default;
    Custom(Custom&&) = default;

    ~Custom() = default;

    Custom& operator = (const Custom&) = default;
    Custom& operator = (Custom&&) = default;
  
    virtual unsigned int size() = 0;
  
  protected:
    std::string tag;
    unsigned long index;

  friend class GraphicsComponent;
  friend class Renderer;
};

template <typename T>
class Data : public Custom
{
  public:
    Data(std::string s, T t) : data(t) { tag = s; }
    Data(const Data&) = default;
    Data(Data&&) = default;

    ~Data() = default;

    Data& operator = (const Data&) = default;
    Data& operator = (Data&&) = default;

    unsigned int size() { return sizeof(T); };

  private:
    T data;

  friend class GraphicsComponent;
  friend class Renderer;
};

class GraphicsComponent
{
  friend class renderer;
  
  public:
    class GraphicsBuilder
    {
      public: 
        GraphicsBuilder(std::string, Material);
        GraphicsBuilder(const GraphicsBuilder&) = delete;
        GraphicsBuilder(GraphicsBuilder&&) = delete;

        ~GraphicsBuilder();

        GraphicsBuilder& operator = (const GraphicsBuilder&) = delete;
        GraphicsBuilder& operator = (GraphicsBuilder&&) = delete;

        GraphicsBuilder& add_model(std::string);
        GraphicsBuilder& add_model(std::vector<VertexData>&, std::vector<unsigned int>&);
        GraphicsBuilder& add_uniform_buffer(std::shared_ptr<Custom>);
        GraphicsBuilder& add_push_constant(std::shared_ptr<Custom>);

        GraphicsComponent& build() const;

      private:
        GraphicsComponent * p_component;
    };
  
  public:
    GraphicsComponent() = default;
    GraphicsComponent(const GraphicsComponent&) = default;
    GraphicsComponent(GraphicsComponent&&) = default;

    ~GraphicsComponent() = default;

    GraphicsComponent& operator = (const GraphicsComponent&) = default;
    GraphicsComponent& operator = (GraphicsComponent&&) = default;

    GraphicsComponent& translate(glm::vec3);
    GraphicsComponent& rotate(float, glm::vec3);
  
  private:
    std::string pipelineTag;
    Material material;

    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 model = glm::mat4(1.0f);

    std::map<std::string, std::shared_ptr<Custom>> uniforms;
    std::map<std::string, std::shared_ptr<Custom>> pushConstants;

  friend class GraphicsBuilder;
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

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vecs::Device&);
  
  private:
    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&) const;
    void choosePresentMode(const vk::raii::PhysicalDevice&) const;
    void chooseExtent(const vk::raii::PhysicalDevice&) const;

    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void createImageViews(const vk::raii::Device&);

  private:
    GLFWwindow * gl_window = nullptr;

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
    Device * device = nullptr;
    
    vk::raii::Instance vk_instance = nullptr;
};

} // namespace vecs

#endif // vecs_hpp