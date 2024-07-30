#include "src/core/include/gui.hpp"
#include "src/core/include/settings.hpp"
#include "vulkan/vulkan_raii.hpp"

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

  float sx, sy;
  glfwGetWindowContentScale(gl_window, &sx, &sy);
  VECS_SETTINGS.update_scale(sx, sy);

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

const vk::raii::ImageView& GUI::depthView() const
{
  return vk_depthView;
}

void GUI::createSurface(const vk::raii::Instance& vk_instance)
{
  vk::raii::SurfaceKHR::CType surface;
  glfwCreateWindowSurface(*vk_instance, gl_window, nullptr, &surface);

  vk_surface = vk::raii::SurfaceKHR(vk_instance, surface);
}

void GUI::setupWindow(const vecs::Device& vecs_device)
{
  chooseSurfaceFormat(vecs_device.physical());
  choosePresentMode(vecs_device.physical());
  chooseExtent(vecs_device.physical());

  createSwapchain(vecs_device.physical(), vecs_device.logical());

  vk_images = vk_swapchain.getImages();
  createImageViews(vecs_device.logical());

  createDepthResources(vecs_device.physical(), vecs_device.logical());
}

void GUI::recreateSwapchain(const vecs::Device& vecs_device)
{
  int width = 0; int height = 0;
  glfwGetFramebufferSize(gl_window, &width, &height);

  while (width == 0 && height == 0)
  {
    glfwGetFramebufferSize(gl_window, &width, &height);
    glfwWaitEvents();
  }

  vecs_device.logical().waitIdle();

  float sx, sy;
  glfwGetWindowContentScale(gl_window, &sx, &sy);
  VECS_SETTINGS.update_scale(sx, sy);

  vk_swapchain.clear();
  vk_images.clear();
  vk_imageViews.clear();

  createSwapchain(vecs_device.physical(), vecs_device.logical());
  vk_images = vk_swapchain.getImages();
  createImageViews(vecs_device.logical());

  createDepthResources(vecs_device.physical(), vecs_device.logical());
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

unsigned int GUI::findIndex(
  const vk::raii::PhysicalDevice& vk_physicalDevice,
  unsigned int filter,
  vk::MemoryPropertyFlags flags
) const
{
  auto properties = vk_physicalDevice.getMemoryProperties();

  for (unsigned long i = 0; i < properties.memoryTypeCount; ++i)
  {
    if ((filter & (1 << i)) &&
        (properties.memoryTypes[i].propertyFlags & flags) == flags)
    {
      return i;
    }
  }

  throw std::runtime_error("error @ vecs::GUI::findIndex() : could not find suitable memory index");
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

void GUI::createDepthResources(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::Device& vk_device)
{
  auto extent = VECS_SETTINGS.extent();
  auto format = VECS_SETTINGS.depth_format();

  vk::ImageCreateInfo ci_image{
    .imageType      = vk::ImageType::e2D,
    .format         = format,
    .extent         = {
      .width  = extent.width,
      .height = extent.height,
      .depth  = 1
    },
    .mipLevels      = 1,
    .arrayLayers    = 1,
    .samples        = vk::SampleCountFlagBits::e1,
    .tiling         = vk::ImageTiling::eOptimal,
    .usage          = vk::ImageUsageFlagBits::eDepthStencilAttachment,
    .sharingMode    = vk::SharingMode::eExclusive,
    .initialLayout  = vk::ImageLayout::eUndefined
  };
  vk_depthImage = vk_device.createImage(ci_image);

  auto requirements = vk_depthImage.getMemoryRequirements();

  vk::MemoryAllocateInfo ai_memory{
    .allocationSize = requirements.size,
    .memoryTypeIndex  = findIndex(
      vk_physicalDevice,
      requirements.memoryTypeBits,
      vk::MemoryPropertyFlagBits::eDeviceLocal
    )
  };
  vk_depthMemory = vk_device.allocateMemory(ai_memory);

  vk_depthImage.bindMemory(*vk_depthMemory, 0);

  vk::ImageViewCreateInfo ci_view{
    .image            = *vk_depthImage,
    .viewType         = vk::ImageViewType::e2D,
    .format           = format,
    .subresourceRange = {
      .aspectMask     = vk::ImageAspectFlagBits::eDepth,
      .baseMipLevel   = 0,
      .levelCount     = 1,
      .baseArrayLayer = 0,
      .layerCount     = 1
    }
  };
  vk_depthView = vk_device.createImageView(ci_view);
}

} // namespace vecs