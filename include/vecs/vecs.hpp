// vecs::vecs_master_hpp version 0.1.4.0 generated on 07-30-2024 19:38:27 with system Darwin

#ifndef vecs_master_hpp
#define vecs_master_hpp

#ifndef vecs_no_includes

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#endif // vecs_no_includes

#include <bitset>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <vector>

#define VECS_LIMIT     std::numeric_limits<unsigned short>::max()
#define VECS_SETTINGS  vecs::Settings::instance()

namespace vecs
{

class IComponentArray;
template <typename T> class ComponentArray;
class ComponentManager;
class Device;
class Engine;
class EntityManager;
class GUI;
class Settings;
class Signature;
class System;
class SystemManager;

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

class IComponentArray
{
  public:
    IComponentArray() = default;
    IComponentArray(const IComponentArray&) = default;
    IComponentArray(IComponentArray&&) = default;

    virtual ~IComponentArray() = default;

    IComponentArray& operator = (const IComponentArray&) = default;
    IComponentArray& operator = (IComponentArray&&) = default;
};

template <typename T>
class ComponentArray : public IComponentArray
{
  public:
    ComponentArray() = default;
    ComponentArray(const ComponentArray&) = default;
    ComponentArray(ComponentArray&&) = default;

    ~ComponentArray() = default;

    ComponentArray& operator = (const ComponentArray&) = default;
    ComponentArray& operator = (ComponentArray&&) = default;
    
    const T& at(unsigned long) const;
    
    void emplace(unsigned long, T);
    void erase(unsigned long);

  protected:
    bool valid(unsigned long) const;

  protected:
    std::vector<T> data;
    std::map<unsigned long, unsigned long> indexMap;
};

class ComponentManager
{
  public:
    ComponentManager() = default;
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager(ComponentManager&&) = delete;

    ~ComponentManager() = default;

    ComponentManager& operator = (const ComponentManager&) = delete;
    ComponentManager& operator = (ComponentManager&&) = delete;
    
    template <typename... Tps>
    void register_components();

    template <typename... Tps>
    void unregister_components();

    template <typename... Tps>
    void update_data(unsigned long, Tps...);

    template <typename... Tps>
    void remove_data(unsigned long);

    template <typename T>
    std::optional<T> retrieve(unsigned long);

    template <typename T>
    bool registered() const;
  
  protected:
    template <typename T>
    void registerComponent();

    template <typename T>
    void unregisterComponent();

    template <typename T>
    void update(unsigned long, T&);

    template <typename T>
    void remove(unsigned long);

    template <typename T>
    std::shared_ptr<ComponentArray<T>> array() const;

  protected:
    std::map<const char *, std::shared_ptr<IComponentArray>> componentMap;

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

        ~QueueFamilies() = default;

        QueueFamilies& operator = (const QueueFamilies&) = delete;
        QueueFamilies& operator = (QueueFamilies&&) = delete;

      private:
        void addFamily(unsigned long, FamilyType);
        void setQueues(const vk::raii::Device&);

      private:
        std::map<std::string, std::shared_ptr<QueueFamily>> familyMap;
        std::vector<FamilyType> supportedFamilies;
    };
  
  public:
    Device(const vk::raii::Instance&, const vecs::GUI&, const void * p_next = nullptr);
    Device(const Device&) = delete;
    Device(Device&&) = delete;

    ~Device() = default;

    Device& operator = (const Device&) = delete;
    Device& operator = (Device&&) = delete;

    const vk::raii::PhysicalDevice& physical() const;
    const vk::raii::Device& logical() const;
    bool hasFamily(FamilyType) const;
    unsigned long familyIndex(FamilyType) const;
    const vk::raii::Queue& queue(FamilyType) const;

  private:
    void getGPU(const vk::raii::Instance&, const vk::raii::SurfaceKHR&);
    void createDevice(const void *);

  private:
    std::unique_ptr<QueueFamilies> queueFamilies = nullptr;

    vk::raii::PhysicalDevice vk_physicalDevice = nullptr;
    vk::raii::Device vk_device = nullptr;
};

class Engine
{
  public:
    Engine();
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;

    ~Engine();

    Engine& operator = (const Engine&) = delete;
    Engine& operator = (Engine&&) = delete;
    
    virtual void load();
    virtual void run();
  
  private:
    void createInstance();

  protected:
    virtual bool close_condition();
    
    bool should_close() const;
    
    void initialize(void * p_next = nullptr);
    void poll_gui();
  
  private:
    vk::raii::Instance vk_instance = nullptr;

  protected:
    std::shared_ptr<GUI> vecs_gui = nullptr;
    std::shared_ptr<Device> vecs_device = nullptr;

    std::unique_ptr<EntityManager> entity_manager = nullptr;
    std::shared_ptr<ComponentManager> component_manager = nullptr;
    std::unique_ptr<SystemManager> system_manager = nullptr;
};

class EntityManager
{
  public:
    EntityManager();
    EntityManager(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;

    ~EntityManager() = default;

    EntityManager& operator = (const EntityManager&) = delete;
    EntityManager& operator = (EntityManager&&) = delete;

    unsigned long count() const;
    bool valid(unsigned long) const;
    
    void new_entity();
    void remove_entity(unsigned long);

    template <typename... Tps>
    std::set<unsigned long> retrieve(bool extactMatch = false) const;

    template <typename... Tps>
    void add_components(unsigned long);

    template <typename... Tps>
    void remove_components(unsigned long);

  protected:
    void sort(unsigned long);
  
  protected:
    std::vector<Signature> signatures;
    std::map<unsigned long, unsigned long> indexMap;
    std::map<unsigned long, unsigned long> idMap;
    std::stack<unsigned long> nextID;
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
    const vk::raii::ImageView& depthView() const;

    void createSurface(const vk::raii::Instance&);
    void setupWindow(const vecs::Device&);
    void recreateSwapchain(const vecs::Device&);

  private:
    static void resizeFramebuffer(GLFWwindow *, int, int);

    void chooseSurfaceFormat(const vk::raii::PhysicalDevice&) const;
    void choosePresentMode(const vk::raii::PhysicalDevice&) const;
    void chooseExtent(const vk::raii::PhysicalDevice&) const;
    unsigned int findIndex(
      const vk::raii::PhysicalDevice&,
      unsigned int,
      vk::MemoryPropertyFlags
    ) const;

    void createSwapchain(const vk::raii::PhysicalDevice&, const vk::raii::Device&);
    void createImageViews(const vk::raii::Device&);
    void createDepthResources(const vk::raii::PhysicalDevice&, const vk::raii::Device&);

  private:
    GLFWwindow * gl_window = nullptr;
    bool modifiedFramebuffer = false;

    vk::raii::SurfaceKHR vk_surface = nullptr;

    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;

    vk::raii::DeviceMemory vk_depthMemory = nullptr;
    vk::raii::Image vk_depthImage = nullptr;
    vk::raii::ImageView vk_depthView = nullptr;
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
    float scale_x() const;
    float scale_y() const;
    float aspect_ratio() const;
    bool portability_enabled() const;
    std::vector<const char *> device_extensions() const;
    vk::Format format() const;
    vk::ColorSpaceKHR color_space() const;
    vk::PresentModeKHR present_mode() const;
    vk::Extent2D extent() const;
    vk::Format depth_format() const;
    unsigned long max_flight_frames() const;
    vk::ClearValue background_color() const;
    const unsigned short& max_entities() const;
    const unsigned short& max_components() const;

    template <typename T>
    unsigned short component_id();

    Settings& update_name(std::string);
    Settings& update_version(unsigned int);
    Settings& toggle_validation();
    Settings& update_title(std::string);
    Settings& update_width(unsigned int);
    Settings& update_height(unsigned int);
    Settings& update_scale(float, float);
    Settings& toggle_portability();
    Settings& add_device_extension(const char *);
    Settings& remove_device_extension(const char *);
    Settings& update_format(vk::Format);
    Settings& update_color_space(vk::ColorSpaceKHR);
    Settings& update_present_mode(vk::PresentModeKHR);
    Settings& update_extent(unsigned int width, unsigned int height);
    Settings& update_max_flight_frames(unsigned long);
    Settings& update_background_color(vk::ClearValue);
    Settings& update_max_entities(unsigned short);
    Settings& update_max_components(unsigned short);

    void set_default();

  private:
    Settings() = default;
    ~Settings() = default;

  private:
    static Settings * p_settings;

    std::map<const char *, unsigned long> s_idMap;
    unsigned int nextID = 0;

    std::string s_name = "VECS Application";
    unsigned int s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    bool s_validationEnabled = true;

    std::string s_title = "VECS Application";
    unsigned int s_width = 1280;
    unsigned int s_height = 720;
    float s_scalex = 1.0f;;
    float s_scaley = 1.0f;

    bool s_portabilityEnabled = false;

    std::vector<const char *> s_gpuExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    vk::Format s_format = vk::Format::eB8G8R8A8Srgb;
    vk::ColorSpaceKHR s_colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    vk::PresentModeKHR s_presentMode = vk::PresentModeKHR::eMailbox;
    vk::Extent2D s_extent{
      .width = s_width,
      .height = s_height
    };

    vk::Format s_dFormat = vk::Format::eD32Sfloat;

    unsigned long s_maxFrames = 2;
    vk::ClearValue s_backColor = vk::ClearValue{
      vk::ClearColorValue{std::array<float, 4>{ 0.0025f, 0.01f, 0.005f, 1.0f }}
    };

    unsigned short s_maxEntities = 20000;
    unsigned short s_maxComponents = 100;
};

class Signature
{  
  public:
    Signature() = default;
    Signature(const Signature&) = default;
    Signature(Signature&&) = default;

    ~Signature() = default;

    Signature& operator = (const Signature&) = default;
    Signature& operator = (Signature&&) = default;
    Signature operator & (const Signature&) const;
    bool operator == (const Signature&) const;

    void reset();

    template <typename... Tps>
    void set();

    template <typename... Tps>
    void unset();
  
  private:
    template <typename T>
    void add();

    template <typename T>
    void remove();

  protected:
    std::bitset<VECS_LIMIT> bits;
};

class System
{
  public:
    System() = default;
    System(const System&) = default;
    System(System&&) = default;

    virtual ~System() = default;

    System& operator = (const System&) = default;
    System& operator = (System&&) = default;

    virtual void update(const std::shared_ptr<ComponentManager>&, std::set<unsigned long>) = 0;
    
    const Signature& signature() const;
    
    template <typename... Tps>
    void addComponents();

    template <typename... Tps>
    void removeComponents();
  
  private:
    Signature sys_signature;
};

class SystemManager
{
  public:
    SystemManager() = default;
    SystemManager(const SystemManager&) = default;
    SystemManager(SystemManager&&) = default;

    ~SystemManager() = default;

    SystemManager& operator = (const SystemManager&) = default;
    SystemManager& operator = (SystemManager&&) = default;

    template <typename T>
    std::optional<std::shared_ptr<T>> system() const;
    
    template <typename... Tps>
    void emplace();

    template <typename... Tps>
    void erase();

    template <typename T, typename... Tps>
    void add_components();

    template <typename T, typename... Tps>
    void remove_components();

  protected:
    template <typename T>
    bool registered() const;

    template <typename T>
    void add();

    template <typename T>
    void remove();

  protected:
    std::map<const char *, std::shared_ptr<System>> systemMap;
};

FamilyType to_family(unsigned int);
unsigned int to_bits(FamilyType);

} // namespace vecs

namespace std
{

std::string to_string(vecs::FamilyType);

} // namspace std

#include "./templates.hpp"
#endif // vecs_master_hpp