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

#include <string>

namespace vecs
{

class Settings
{ 
  public:
    static Settings& instance();

    std::string name() const;
    unsigned int version() const;
    bool validation_enabled() const;
    std::string title() const;
    unsigned int width() const;
    unsigned int height() const;
    bool portability_enabled() const;

    Settings& update_name(std::string);
    Settings& update_version(unsigned int);
    Settings& toggle_validation();
    Settings& update_title(std::string);
    Settings& update_width(unsigned int);
    Settings& update_height(unsigned int);
    Settings& toggle_portability();
    
    void set_default();

  private:
    Settings() = default;
  
  private:
    std::string s_name = "VECS Application";
    unsigned int s_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    bool s_validationEnabled = true;
    
    std::string s_title = "VECS Application";
    unsigned int s_width = 1280;
    unsigned int s_height = 720;

    bool s_portabilityEnabled = false;
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

  private:
    GLFWwindow * gl_window = nullptr;
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
    
    vk::raii::Instance vk_instance = nullptr;
};

} // namespace vecs

#endif // vecs_hpp