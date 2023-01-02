#ifndef PLEX_GRAPHICS_VULKAN_VULKAN_SWAPCHAIN_H
#define PLEX_GRAPHICS_VULKAN_VULKAN_SWAPCHAIN_H

#include "plex/graphics/renderer.h"
#include "plex/graphics/vulkan/api/vulkan_api.h"
#include "plex/graphics/vulkan/vulkan_device.h"
#include "plex/graphics/vulkan/vulkan_surface.h"

namespace plex::graphics
{
class VulkanSwapchain
{
public:
  VulkanSwapchain(VulkanDevice* device,
    VulkanSurface* surface,
    PresentMode present_mode,
    uint32_t width,
    uint32_t height,
    uint32_t image_count);

  ~VulkanSwapchain();

  VulkanSwapchain(const VulkanSwapchain&) = delete;
  VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;
  VulkanSwapchain(VulkanSwapchain&&) = delete;
  VulkanSwapchain& operator=(VulkanSwapchain&&) = delete;

  void CreateFrameBuffers(VkRenderPass render_pass);

  [[nodiscard]] uint32_t AquireNextImage(VkSemaphore semaphore);

  void Present(uint32_t image_index, VkQueue queue, VkSemaphore semaphore);

  [[nodiscard]] VkImage GetImage(uint32_t index) const noexcept
  {
    return images_[index];
  }

  [[nodiscard]] VkImageView GetImageView(uint32_t index) const noexcept
  {
    return image_views_[index];
  }

  [[nodiscard]] VkFramebuffer GetFramebuffer(uint32_t index) const noexcept
  {
    return framebuffers_[index];
  }

  [[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat() const noexcept
  {
    return surface_format_;
  }

  [[nodiscard]] VkExtent2D GetExtent() const noexcept
  {
    return extent_;
  }

  [[nodiscard]] uint32_t GetImageCount() const noexcept
  {
    return image_count_;
  }

  [[nodiscard]] VkSwapchainKHR GetHandle() const noexcept
  {
    return swapchain_;
  }

private:
  VkSwapchainKHR swapchain_;
  VkDevice device_;

  VkSurfaceFormatKHR surface_format_;
  VkPresentModeKHR present_mode_;
  VkExtent2D extent_;
  uint32_t image_count_;

  std::vector<VkImage> images_;
  std::vector<VkImageView> image_views_;
  std::vector<VkFramebuffer> framebuffers_;
};
} // namespace plex::graphics

#endif