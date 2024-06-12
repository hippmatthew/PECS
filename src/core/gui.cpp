#include "src/core/include/gui.hpp"

namespace pecs
{

GUI::GUI()
{
  initialize();
}

GUI::GUI(const Settings::GUI& s)
{
  settings = s;
  
  initialize();
}

GUI::~GUI()
{
  glfwDestroyWindow(gl_window);
  glfwTerminate();
}

std::vector<const char *> GUI::extensions() const
{
  unsigned int extensionCount = 0;
  const char ** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

  return std::vector(extensions, extensions + extensionCount);
}

GLFWwindow * GUI::window() const
{
  return gl_window;
}

const vk::raii::SurfaceKHR& GUI::surface() const
{
  return vk_surface;
}

const vk::Extent2D& GUI::extent() const
{
  return vk_extent;
}

const vk::Format& GUI::format() const
{
  return vk_surfaceFormat.format;
}

const vk::raii::SwapchainKHR& GUI::swapchain() const
{
  return vk_swapchain;
}

const vk::Image& GUI::image(const unsigned int& index) const
{
  return vk_images[index];
}

const vk::raii::ImageView& GUI::imageView(const unsigned int& index) const
{
  return vk_imageViews[index];
}

void GUI::createSurface(const vk::raii::Instance& vk_instance)
{
  vk::raii::SurfaceKHR::CType cSurface;
  glfwCreateWindowSurface(*vk_instance, gl_window, nullptr, &cSurface);

  vk_surface = vk::raii::SurfaceKHR(vk_instance, cSurface); 
}

void GUI::setupWindow(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::Device& vk_device)
{
  chooseSurfaceFormat(vk_physicalDevice);
  choosePresentMode(vk_physicalDevice);
  chooseExtent(vk_physicalDevice);
  
  createSwapchain(vk_physicalDevice, vk_device);
  
  vk_images = vk_swapchain.getImages();
  createImageViews(vk_device);
}

void GUI::recreateSwapchain(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::Device& vk_device)
{
  int width = 0, height = 0;
  glfwGetFramebufferSize(gl_window, &width, &height);
  while (width == 0 && height == 0)
  {
    glfwGetFramebufferSize(gl_window, &width, &height);
    glfwWaitEvents();
  }
  
  vk_device.waitIdle();

  vk_swapchain.clear();
  vk_imageViews.clear();

  createSwapchain(vk_physicalDevice, vk_device);
  createImageViews(vk_device);
}

void GUI::initialize()
{
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  gl_window = glfwCreateWindow(settings.width, settings.height, settings.windowTitle.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(gl_window, this);
  glfwSetFramebufferSizeCallback(gl_window, resizeFramebuffer);
}

void GUI::createSwapchain(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::Device& vk_device)
{
  auto surfaceCapabilities = vk_physicalDevice.getSurfaceCapabilitiesKHR(*vk_surface);
  
  unsigned int imageCount = surfaceCapabilities.minImageCount + 1;
  if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    imageCount = surfaceCapabilities.maxImageCount;

  vk::SwapchainCreateInfoKHR ci_swapchain{
    .surface                = *vk_surface,
    .minImageCount          = imageCount,
    .imageFormat            = vk_surfaceFormat.format,
    .imageColorSpace        = vk_surfaceFormat.colorSpace,
    .imageExtent            = vk_extent,
    .imageArrayLayers       = 1,
    .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode       = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount  = 0,
    .pQueueFamilyIndices    = nullptr,
    .preTransform           = surfaceCapabilities.currentTransform,
    .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode            = vk_presentMode,
    .clipped                = VK_TRUE,
    .oldSwapchain           = VK_NULL_HANDLE
  };

  vk_swapchain = vk_device.createSwapchainKHR(ci_swapchain);
}

void GUI::chooseSurfaceFormat(const vk::raii::PhysicalDevice& vk_physicalDevice)
{
  bool found = false;
  auto surfaceFormats = vk_physicalDevice.getSurfaceFormatsKHR(*vk_surface);
  for (const auto& surfaceFormat : surfaceFormats)
  {
    if (surfaceFormat.format == vk::Format::eB8G8R8A8Srgb && surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
    {
      vk_surfaceFormat = surfaceFormat;
      found = true;
      break;
    }
  }
  if (!found) vk_surfaceFormat = surfaceFormats[0];
}

void GUI::choosePresentMode(const vk::raii::PhysicalDevice& vk_physicalDevice)
{
  bool found = false;
  for (const auto& presentMode : vk_physicalDevice.getSurfacePresentModesKHR(*vk_surface))
  {
    if (presentMode == vk::PresentModeKHR::eMailbox)
    {
      vk_presentMode = presentMode;
      found = true;
      break;
    }
  }
  if (!found) vk_presentMode = vk::PresentModeKHR::eFifo;
}

void GUI::chooseExtent(const vk::raii::PhysicalDevice& vk_physicalDevice)
{
  auto surfaceCapabilities = vk_physicalDevice.getSurfaceCapabilitiesKHR(*vk_surface);

  if (surfaceCapabilities.currentExtent.width != std::numeric_limits<unsigned int>::max())
  {
    vk_extent = surfaceCapabilities.currentExtent;
    return;
  }

  int width, height;
  glfwGetFramebufferSize(gl_window, &width, &height);

  vk_extent.width = static_cast<unsigned int>(width);
  vk_extent.height = static_cast<unsigned int>(height);
 
  vk_extent.width = std::clamp(vk_extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
  vk_extent.height = std::clamp(vk_extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
}

void GUI::createImageViews(const vk::raii::Device& vk_device)
{
  vk::ComponentMapping components{
    .r = vk::ComponentSwizzle::eIdentity,
    .g = vk::ComponentSwizzle::eIdentity,
    .b = vk::ComponentSwizzle::eIdentity,
    .a = vk::ComponentSwizzle::eIdentity
  };

  vk::ImageSubresourceRange subresourceRange{
    .aspectMask     = vk::ImageAspectFlagBits::eColor,
    .baseMipLevel   = 0,
    .levelCount     = 1,
    .baseArrayLayer = 0,
    .layerCount     = 1
  };

  for (const auto& vk_image : vk_images)
  {
    vk::ImageViewCreateInfo ci_imageView{
      .image            = vk_image,
      .viewType         = vk::ImageViewType::e2D,
      .format           = vk_surfaceFormat.format,
      .components       = components,
      .subresourceRange = subresourceRange
    };

    vk_imageViews.emplace_back(vk_device.createImageView(ci_imageView));
  }
}

void GUI::resizeFramebuffer(GLFWwindow * window, int width, int height)
{
  auto gui = reinterpret_cast<GUI *>(glfwGetWindowUserPointer(window));
  gui->framebufferChanged = true;
}

} // namespace pecs