#ifndef PLEX_GRAPHICS_VULKAN_RENDERER_H
#define PLEX_GRAPHICS_VULKAN_RENDERER_H

#include "plex/graphics/renderer.h"
#include "plex/graphics/vulkan/pipeline/vulkan_compute_pipeline.h"
#include "plex/graphics/vulkan/shaders/vulkan_shader_program.h"
#include "plex/graphics/vulkan/vulkan_device.h"
#include "plex/graphics/vulkan/vulkan_surface.h"
#include "plex/graphics/vulkan/vulkan_swapchain.h"

#include "plex/debug/logging.h"

#include <memory>

namespace plex
{

// TODO window resize event -> resize buffers, swapchain, etc...
class VulkanRenderer : public Renderer
{
public:
  VulkanRenderer(std::shared_ptr<Window> window, const std::string& application_name, GraphicsDebugLevel debug_level)
    : window_(window)
  {
    if (dynamic_cast<VulkanCapableWindow*>(window.get()) == nullptr)
    {
      LOG_ERROR("Window is not vulkan capable");

      return;
    }

    if (!Initialize(application_name, debug_level))
    {
      LOG_ERROR("Failed to initialize vulkan renderer");

      return;
    }

    LOG_INFO("Vulkan renderer initialized");
  }

  ~VulkanRenderer() override
  {

    vkDestroyDescriptorSetLayout(device_->GetHandle(), descriptor_set_layout_, nullptr);
    LOG_INFO("Vulkan descriptor set layout destroyed");
    vkDestroyCommandPool(device_->GetHandle(), command_pool_, nullptr);
    LOG_INFO("Vulkan command pool destroyed");
    vkDestroyDescriptorPool(device_->GetHandle(), descriptor_pool_, nullptr);
    LOG_INFO("Vulkan descriptor pool destroyed");
    LOG_INFO("Vulkan renderer destroyed");
  }

  void Draw(uint32_t frame_index) override
  {
    auto fence_wait_result =
      vkWaitForFences(device_->GetHandle(), 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);
    if (fence_wait_result != VK_SUCCESS)
    {
      LOG_ERROR("Failed to wait for fence");
    }

    VkAcquireNextImageInfoKHR acquire_info {};
    acquire_info.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
    acquire_info.swapchain = swapchain_->GetHandle();
    acquire_info.timeout = 10'000'000'000; // 10 seconds
    acquire_info.semaphore = image_available_semaphores_[current_frame_];
    acquire_info.fence = VK_NULL_HANDLE;
    acquire_info.deviceMask = 1;

    uint32_t swapchain_index;
    VkResult acquire_result = vkAcquireNextImage2KHR(device_->GetHandle(), &acquire_info, &swapchain_index);
    if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
    {
      // TODO recreate swapchain
    }
    else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
    {
      LOG_ERROR("Failed to acquire swapchain image");
    }

    if (images_in_flight_[current_frame_] != VK_NULL_HANDLE)
    {
      vkWaitForFences(device_->GetHandle(), 1, &images_in_flight_[current_frame_], VK_TRUE, UINT64_MAX);
    }

    images_in_flight_[current_frame_] = in_flight_fences_[current_frame_];

    push_constant_data_.width = static_cast<uint32_t>(swapchain_->GetExtent().width);
    push_constant_data_.height = static_cast<uint32_t>(swapchain_->GetExtent().height);
    push_constant_data_.swapchain_index = swapchain_index;
    push_constant_data_.frame_index = frame_index;
    push_constant_data_.color[0] = 0.0f;
    push_constant_data_.color[1] = 0.0f;
    push_constant_data_.color[2] = .5f;
    push_constant_data_.color[3] = 1.0f;

    VkCommandBufferBeginInfo command_buffer_begin_info {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = 0;

    const auto command_buffer = command_buffers_[current_frame_];

    vkResetCommandBuffer(command_buffer, 0);

    if (vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info) != VK_SUCCESS)
    {
      LOG_ERROR("Failed to begin command buffer");
    }

    vkCmdSetDeviceMask(command_buffer, 1);

    // Image memory barrier to make sure that compute shader writes are finished before sampling from the texture
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    // We won't be changing the layout of the image
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier.image = swapchain_->GetImages()[swapchain_index];
    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    vkCmdPipelineBarrier(command_buffer,
      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0,
      0,
      nullptr,
      0,
      nullptr,
      1,
      &imageMemoryBarrier);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline_->GetHandle());

    vkCmdBindDescriptorSets(command_buffer,
      VK_PIPELINE_BIND_POINT_COMPUTE,
      compute_pipeline_->GetLayout(),
      0,
      1,
      &descriptor_set_,
      0,
      nullptr);

    vkCmdPushConstants(command_buffer,
      compute_pipeline_->GetLayout(),
      VK_SHADER_STAGE_COMPUTE_BIT,
      0,
      (uint32_t)sizeof(PushConstantData),
      &push_constant_data_);
    vkCmdDispatch(command_buffer, 1, 1, 1);

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
    {
      LOG_ERROR("Failed to end command buffer");
    }

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
    submit_info.pWaitDstStageMask = wait_stages;

    VkSemaphore wait_semaphores[] = { image_available_semaphores_[current_frame_] };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;

    VkSemaphore signal_semaphores[] = { render_finished_semaphores_[current_frame_] };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(device_->GetHandle(), 1, &in_flight_fences_[current_frame_]);

    vkQueueSubmit(device_->GetComputeQueue(), 1, &submit_info, in_flight_fences_[current_frame_]);
    // vkQueueWaitIdle(device_->GetComputeQueue());

    VkPresentInfoKHR present_info {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = { swapchain_->GetHandle() };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &swapchain_index;
    present_info.pResults = nullptr; // Optional

    auto result = vkQueuePresentKHR(device_->GetPresentQueue(), &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
      LOG_INFO("Swapchain out of date");
    }
    else if (result != VK_SUCCESS)
    {
      LOG_ERROR("Failed to present swapchain image");
    }

    current_frame_ = (current_frame_ + 1) % swapchain_->GetImageCount();

    // when implementing the abstraction for the command buffer, use the stream operator to write to the command buffer
    // then a .flush() call will be made to submit the command buffer to the device
    // additionally, add an immediate mode.
    // the stream will gather all data necessary for the push constants.
  }

  /*
 An image is just a piece of memory with some metadata about layout, format etc. A Framebuffer is a container for
 multiple images with additional metadata for each image, like usage, identifier(index) and type(color, depth, etc.).
 These images used in a framebuffer are called attachments, because they are attached to, and owned by, a framebuffer.

 Attachments that get rendered to are called Render Targets, Attachments which are used as input are called Input
 Attachments.

 Attachments which hold information about Multisampling are called Resolve Attachments.
 Attachments with RGB/Depth/Stencil information are calledColor/Depth/Stencil Attachments respectively.
 */

  /*
   Attachments are just pixel local render buffers while Image that are not attached to a framebuffer can be sampled
   */

private:
  struct PushConstantData
  {
    uint32_t width : 14, height : 14, swapchain_index : 4;
    uint32_t frame_index;
    float color[4];
   // uint32_t padding[2];
  };

  bool Initialize(const std::string& application_name, GraphicsDebugLevel debug_level)
  {
    instance_ = std::make_shared<VulkanInstance>(window_, application_name, debug_level);
    surface_ = std::make_shared<VulkanSurface>(window_, instance_);
    device_ = std::make_shared<VulkanDevice>(instance_, surface_, SwapchainImageUsage::StorageImage);
    swapchain_ = std::make_shared<VulkanSwapchain>(window_, device_, surface_, SwapchainImageUsage::StorageImage);

    std::vector<PushRange> push_ranges;
    push_ranges.emplace_back(PushRangeStage::Compute, (uint32_t)sizeof(PushConstantData), 0);
    // push_ranges.emplace_back(PushRangeStage::Compute, sizeof(uint64_t), sizeof(uint64_t));

    VkCommandPoolCreateInfo command_pool_info {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.queueFamilyIndex = device_->GetQueueFamilyIndices().compute_family_index;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device_->GetHandle(), &command_pool_info, nullptr, &command_pool_) != VK_SUCCESS)
    {
      LOG_ERROR("Failed to create command pool");
    }

    LOG_INFO("Vulkan command pool created");

    VkDescriptorSetLayoutBinding layout_binding {};
    layout_binding.binding = 0;
    layout_binding.descriptorCount = 42069; // yeet
    layout_binding.pImmutableSamplers = VK_NULL_HANDLE;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_info {};
    binding_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    binding_info.bindingCount = 1;

    const VkDescriptorBindingFlagsEXT flags =
      VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT
      | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;
    binding_info.pBindingFlags = &flags;

    VkDescriptorSetLayoutCreateInfo layout_info {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &layout_binding;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
    layout_info.pNext = &binding_info;

    if (vkCreateDescriptorSetLayout(device_->GetHandle(), &layout_info, nullptr, &descriptor_set_layout_) != VK_SUCCESS)
    {
      LOG_ERROR("Failed to create descriptor set layout");
    }

    LOG_INFO("Vulkan descriptor set layout created");

    compute_pipeline_ = std::make_shared<VulkanComputePipeline>(device_, push_ranges, 1, descriptor_set_layout_);
    const std::string shader_path = "./../../sandbox/basic_window/shaders/";
    const auto compute_path = shader_path + "shader.comp.spv";
    auto compute_program = std::make_shared<VulkanShaderProgram>(device_, compute_pipeline_, compute_path);

    // Swapchain
    VkDescriptorPoolSize pool_size {};
    pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    pool_size.descriptorCount = 1;

    VkDescriptorPoolCreateInfo pool_info {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
    pool_info.maxSets = 1;

    if (vkCreateDescriptorPool(device_->GetHandle(), &pool_info, nullptr, &descriptor_pool_) != VK_SUCCESS)
    {
      LOG_ERROR("Failed to create descriptor pool");
    }

    LOG_INFO("Vulkan descriptor pool created");

    VkDescriptorSetVariableDescriptorCountAllocateInfoEXT variable_info {};
    variable_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO_EXT;
    variable_info.descriptorSetCount = 1;

    uint32_t variable_desc_counts[] = { swapchain_->GetImageCount() };
    variable_info.pDescriptorCounts = variable_desc_counts;

    VkDescriptorSetAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = descriptor_pool_;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout_;
    alloc_info.pNext = &variable_info;

    if (vkAllocateDescriptorSets(device_->GetHandle(), &alloc_info, &descriptor_set_) != VK_SUCCESS)
    {
      LOG_ERROR("Failed to allocate descriptor set");
    }

    LOG_INFO("Vulkan descriptor set allocated");

    VkWriteDescriptorSet descriptor_write {};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = descriptor_set_;
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptor_write.descriptorCount = swapchain_->GetImageCount();

    const auto& attachment_group = swapchain_->GetAttachments(0);

    std::vector<VkDescriptorImageInfo> image_infos(swapchain_->GetImageCount());
    for (uint32_t i = 0; i < swapchain_->GetImageCount(); ++i)
    {
      image_infos[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
      image_infos[i].imageView = attachment_group.GetImageView(i);
      image_infos[i].sampler = VK_NULL_HANDLE;
    }

    descriptor_write.pImageInfo = image_infos.data();

    vkUpdateDescriptorSets(device_->GetHandle(), 1, &descriptor_write, 0, nullptr);

    LOG_INFO("Vulkan descriptor set updated");

    VkCommandBufferAllocateInfo command_buffer_allocate_info {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.commandPool = command_pool_;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = swapchain_->GetImageCount();

    command_buffers_.resize(swapchain_->GetImageCount());
    if (vkAllocateCommandBuffers(device_->GetHandle(), &command_buffer_allocate_info, command_buffers_.data())
        != VK_SUCCESS)
    {
      LOG_ERROR("Failed to allocate command buffers");
    }

    LOG_INFO("Vulkan command buffers allocated");

    VkSemaphoreCreateInfo semaphore_create_info {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_create_info {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    image_available_semaphores_.resize(swapchain_->GetImageCount());
    render_finished_semaphores_.resize(swapchain_->GetImageCount());
    in_flight_fences_.resize(swapchain_->GetImageCount());
    images_in_flight_.resize(swapchain_->GetImageCount(), VK_NULL_HANDLE);
    for (uint32_t i = 0; i < swapchain_->GetImageCount(); ++i)
    {
      if (vkCreateSemaphore(device_->GetHandle(), &semaphore_create_info, nullptr, &image_available_semaphores_[i])
          != VK_SUCCESS)
      {
        LOG_ERROR("Failed to create semaphore");
      }

      if (vkCreateSemaphore(device_->GetHandle(), &semaphore_create_info, nullptr, &render_finished_semaphores_[i])
          != VK_SUCCESS)
      {
        LOG_ERROR("Failed to create semaphore");
      }

      if (vkCreateFence(device_->GetHandle(), &fence_create_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
      {
        LOG_ERROR("Failed to create fence");
      }
    }

    // descriptor_write.pImageInfo = &swapchain_->GetImageInfo();

    //    TextureManager::Create(device_);
    //
    //    struct Vertex
    //    {
    //      float position[3];
    //      float color[3];
    //      float uv[2];
    //    };
    //
    //    auto vertex_buffer = std::make_shared<VulkanVertexBuffer>(device_, 0, sizeof(Vertex));
    //    vertex_buffer->AddAttribute(VertexAttributeType::FloatVector3, offsetof(Vertex, position));
    //    vertex_buffer->AddAttribute(VertexAttributeType::FloatVector3, offsetof(Vertex, color));
    //    vertex_buffer->AddAttribute(VertexAttributeType::FloatVector2, offsetof(Vertex, uv));
    //
    //    auto pipeline = std::make_shared<VulkanPipeline>(device_);
    //
    //    const std::string shader_path = "./../../sandbox/basic_window/shaders/";
    //    const auto vertex_path = shader_path + "basic_window.vert.spv";
    //    const auto fragment_path = shader_path + "basic_window.frag.spv";
    //    auto ShaderProgram = std::make_shared<VulkanShaderProgram>(device_, pipeline, vertex_path, fragment_path);
    //
    //    auto swapchain_extent_ = swapchain_->GetExtent();
    //    auto depth_attachment = std::make_shared<VulkanDepthAttachment>(device_, 0, swapchain_extent_);
    //    auto color_image = std::make_shared<VulkanColorAttachment>(device_, 1, swapchain_extent_);
    //
    //    const auto render_pass_1 = std::make_shared<VulkanRenderPass>(device_);
    //    const auto depth_image_handle = render_pass_1->AddImage(depth_attachment);
    //    const auto color_image_handle = render_pass_1->AddImage(color_image);
    //    const auto swapchain_image_handle = render_pass_1->AddSwapchainImages(0, swapchain_);
    //
    //    const auto subpass_1 = render_pass_1->AddSubPass();
    //    render_pass_1->AddSubPassInput(subpass_1, depth_image_handle);
    //    render_pass_1->AddSubPassOutput(subpass_1, depth_image_handle);
    //    render_pass_1->AddSubPassOutput(subpass_1, color_image_handle);
    //
    //    const auto subpass_2 = render_pass_1->AddSubPass();
    //    // TODO replace render_pass->AddSubPassInput by proxy object and use subpass_proxy->writes_to(image_handle)
    //    and
    //    //  subpass_proxy->reads_from(image_handle)
    //    render_pass_1->AddSubPassInput(subpass_2, color_image_handle);
    //    render_pass_1->AddSubPassInput(subpass_2, swapchain_image_handle);
    //    render_pass_1->AddSubPassOutput(subpass_2, swapchain_image_handle);
    //
    //    const auto render_pass_handle = render_pass_1->GetHandle();
    //
    //    const std::shared_ptr<VulkanRenderPass>& render_pass_2_ = std::make_shared<VulkanRenderPass>(device_);
    //    const auto swapchain_images_handle = render_pass_2_->AddSwapchainImages(0, swapchain_);

    // Renderer::Submit() or Renderer::Flush();
    //  Pseudo code:
    /*
      RenderPass creation
      Set Render target -> swapchain -> handle
      Add per frame Color Attachment -> handle
      Add shared depth attachment -> handle

      //define subpasses (auto-generated)
      use swapchain handle
        use color attachment handle
        use depth attachment handle

    */

    /*
     attachment handle 64 bits
     32 bits render pass
     32 bits subpass
     inside a class, so they can't be modified
     */

    // https://www.reddit.com/r/vulkan/comments/4gvmus/best_way_for_textures_in_shaders/

    /*

     Fragment only not supported yet as it would be better to do a compute shader instead

     ShaderProgram
     -> FragmentShader
     -> VertexShader

     SwapchainImage scImg = SwapchainImage->AcquireNextImage();

     ColorImage img1

     attachment index ----V
     SP1->ReadsFrom(img1, 0, VERTEX_SHADER_STAGE) // unfiltered by default
     SP1->WritesTo(img1, 0)

     or

     ColorImage img2
     SP1->WritesTo(img2, 0)



     SP1->ReadsFromFiltered(img1, VERTEX_SHADER_STAGE, ?FILTER_MODE?)

     */

    return true;
  }

private:
  std::shared_ptr<Window> window_;

  std::shared_ptr<VulkanInstance> instance_;
  std::shared_ptr<VulkanSurface> surface_;
  std::shared_ptr<VulkanDevice> device_;
  std::shared_ptr<VulkanSwapchain> swapchain_;

  VkCommandPool command_pool_;
  VkDescriptorPool descriptor_pool_;
  VkDescriptorSetLayout descriptor_set_layout_;
  std::shared_ptr<VulkanComputePipeline> compute_pipeline_;
  VkDescriptorSet descriptor_set_;

  std::vector<VkCommandBuffer> command_buffers_;
  VkSemaphore image_available_semaphore_;
  VkSemaphore render_finished_semaphore_;
  VkFence render_finished_fence_;

  std::vector<VkSemaphore> image_available_semaphores_;
  std::vector<VkSemaphore> render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight_;

  uint32_t current_frame_ = 0;
  PushConstantData push_constant_data_;
};

} // namespace plex
#endif
