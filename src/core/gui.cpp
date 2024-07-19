#include "src/core/include/gui.hpp"

namespace vecs
{

GUI::GUI()
{  
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  gl_window = glfwCreateWindow(
    VECS_SETTINGS.width(),
    VECS_SETTINGS.height(),
    VECS_SETTINGS.title().c_str(),
    nullptr,
    nullptr
  );

  glfwSetWindowUserPointer(gl_window, this);
  glfwSetFramebufferSizeCallback(gl_window, resizeFramebuffer);
};

GUI::~GUI()
{
  glfwDestroyWindow(gl_window);
  glfwTerminate();
}

std::vector<const char *> GUI::extensions() const
{
  unsigned int extensionCount = 0;
  const char ** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

  return std::vector<const char *>(extensions, extensions + extensionCount);
}

bool GUI::shouldClose() const
{
  return glfwWindowShouldClose(gl_window);
}

void GUI::pollEvents() const
{
  glfwPollEvents();
}

const vk::raii::SurfaceKHR& GUI::surface() const
{
  return vk_surface;
}

const vk::raii::SwapchainKHR& GUI::swapchain() const
{
  return vk_swapchain;
}

const vk::Image& GUI::image(unsigned long index) const
{
  return vk_images[index];
}

const vk::raii::ImageView& GUI::imageView(unsigned long index) const
{
  return vk_imageViews[index];
}

void GUI::createSurface(const vk::raii::Instance& vk_instance)
{
  vk::raii::SurfaceKHR::CType surface;
  glfwCreateWindowSurface(*vk_instance, gl_window, nullptr, &surface);
  
  vk_surface = vk::raii::SurfaceKHR(vk_instance, surface);
}

void GUI::setupWindow(const vecs::Device& device)
{
  chooseSurfaceFormat(device.physical());
  choosePresentMode(device.physical());
  chooseExtent(device.physical());

  createSwapchain(device.physical(), device.logical());

  vk_images = vk_swapchain.getImages();
  createImageViews(device.logical());
}

void GUI::recreateSwapchain(const vecs::Device& device)
{
  int width = 0; int height = 0;
  glfwGetFramebufferSize(gl_window, &width, &height);

  while (width == 0 && height == 0)
  {
    glfwGetFramebufferSize(gl_window, &width, &height);
    glfwWaitEvents();
  }

  device.logical().waitIdle();

  vk_swapchain.clear();
  vk_images.clear();
  vk_imageViews.clear();

  createSwapchain(device.physical(), device.logical());
  vk_images = vk_swapchain.getImages();
  createImageViews(device.logical());
}

void GUI::resizeFramebuffer(GLFWwindow * window, int width, int height)
{
  auto gui = reinterpret_cast<GUI *>(glfwGetWindowUserPointer(window));
  gui->modifiedFramebuffer = true;
}

void GUI::chooseSurfaceFormat(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto formats = vk_physicalDevice.getSurfaceFormatsKHR(*vk_surface);
  for (const auto& format : formats)
  {
    if (format.format == VECS_SETTINGS.format() && format.colorSpace == VECS_SETTINGS.color_space())
      return;
  }
  VECS_SETTINGS.update_format(formats[0].format);
  VECS_SETTINGS.update_color_space(formats[0].colorSpace);
}

void GUI::choosePresentMode(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto modes = vk_physicalDevice.getSurfacePresentModesKHR(*vk_surface);
  for (const auto& mode : modes)
  {
    if (mode == VECS_SETTINGS.present_mode())
      return;
  }
  VECS_SETTINGS.update_present_mode(vk::PresentModeKHR::eFifo);
}

void GUI::chooseExtent(const vk::raii::PhysicalDevice& vk_physicalDevice) const
{
  auto surfaceCapabilities = vk_physicalDevice.getSurfaceCapabilitiesKHR(*vk_surface);

  if (surfaceCapabilities.currentExtent.width != std::numeric_limits<unsigned int>::max())
  {
    VECS_SETTINGS.update_extent(surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
    return;
  }

  int width, height;
  glfwGetFramebufferSize(gl_window, &width, &height);

  VECS_SETTINGS.update_extent(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
  VECS_SETTINGS.update_extent(
    std::clamp(VECS_SETTINGS.width(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
    std::clamp(VECS_SETTINGS.height(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
  );
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
    .imageFormat            = VECS_SETTINGS.format(),
    .imageColorSpace        = VECS_SETTINGS.color_space(),
    .imageExtent            = VECS_SETTINGS.extent(),
    .imageArrayLayers       = 1,
    .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode       = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount  = 0,
    .pQueueFamilyIndices    = nullptr,
    .preTransform           = surfaceCapabilities.currentTransform,
    .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode            = VECS_SETTINGS.present_mode(),
    .clipped                = vk::True,
    .oldSwapchain           = nullptr
  };

  vk_swapchain = vk_device.createSwapchainKHR(ci_swapchain);
}

void GUI::createImageViews(const vk::raii::Device& vk_device)
{
  vk::ComponentMapping components{
    .r  = vk::ComponentSwizzle::eIdentity,
    .g  = vk::ComponentSwizzle::eIdentity,
    .b  = vk::ComponentSwizzle::eIdentity,
    .a  = vk::ComponentSwizzle::eIdentity
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
      .format           = VECS_SETTINGS.format(),
      .components       = components,
      .subresourceRange = subresourceRange
    };

    vk_imageViews.emplace_back(vk_device.createImageView(ci_imageView));
  }
}

} // namespace vecs