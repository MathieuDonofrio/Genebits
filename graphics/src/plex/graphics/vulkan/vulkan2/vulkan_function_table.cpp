
#include "vulkan_function_table.h"

#include "plex/debug/logging.h"

// clang-format off

namespace plex::graphics::vkapi::loader
{

namespace
{
 static FunctionTable _table; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
 static std::vector<const char*> _extensions; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
} // namespace

FunctionTable& GetFunctionTable() noexcept
{
    return _table;
};

void SetExtensions(const std::vector<const char*>& extensions)
{
    _extensions = extensions;
};

const std::vector<const char*>& GetExtensions() noexcept
{
    return _extensions;
};

namespace
{
using FunctionName = std::string;
const std::tuple<FunctionName, FunctionGroupStartIndex, FunctionGroupCount> offset_list[] = { // NOLINT(cert-err58-cpp)
  {"vkGetInstanceProcAddr", 0, 1},
  {"global", 1, 4},
  {"core", 5, 210},
  {"VK_KHR_surface", 215, 5},
  {"VK_KHR_swapchain", 220, 9},
  {"VK_KHR_display", 229, 7},
  {"VK_KHR_display_swapchain", 236, 1},
  {"VK_KHR_sampler_mirror_clamp_to_edge", 237, 2},
  {"VK_KHR_multiview", 239, 7},
  {"VK_KHR_device_group", 246, 3},
  {"VK_KHR_shader_draw_parameters", 249, 1},
  {"VK_KHR_device_group_creation", 250, 1},
  {"VK_KHR_external_memory_capabilities", 251, 1},
  {"VK_KHR_external_memory", 252, 2},
  {"VK_KHR_external_semaphore_capabilities", 254, 1},
  {"VK_KHR_external_semaphore", 255, 2},
  {"VK_KHR_push_descriptor", 257, 2},
  {"VK_KHR_shader_float16_int8", 259, 3},
  {"VK_KHR_imageless_framebuffer", 262, 4},
  {"VK_KHR_shared_presentable_image", 266, 1},
  {"VK_KHR_external_fence_capabilities", 267, 1},
  {"VK_KHR_external_fence", 268, 2},
  {"VK_KHR_performance_query", 270, 4},
  {"VK_KHR_maintenance2", 274, 2},
  {"VK_KHR_variable_pointers", 276, 4},
  {"VK_KHR_dedicated_allocation", 280, 3},
  {"VK_KHR_image_format_list", 283, 2},
  {"VK_KHR_bind_memory2", 285, 2},
  {"VK_KHR_maintenance3", 287, 1},
  {"VK_KHR_draw_indirect_count", 288, 2},
  {"VK_KHR_shader_subgroup_extended_types", 290, 3},
  {"VK_KHR_vulkan_memory_model", 293, 2},
  {"VK_KHR_spirv_1_4", 295, 1},
  {"VK_KHR_uniform_buffer_standard_layout", 296, 3},
  {"VK_KHR_deferred_host_operations", 299, 5},
  {"VK_KHR_pipeline_executable_properties", 304, 3},
  {"VK_KHR_shader_integer_dot_product", 307, 8},
  {"VK_KHR_shader_subgroup_uniform_control_flow", 315, 6},
  {"VK_KHR_format_feature_flags2", 321, 3},
  {"VK_EXT_debug_report", 324, 3},
  {"VK_NV_glsl_shader", 327, 5},
  {"VK_AMD_gcn_shader", 332, 6},
  {"VK_NVX_binary_import", 338, 5},
  {"VK_NVX_image_view_handle", 343, 2},
  {"VK_AMD_draw_indirect_count", 345, 2},
  {"VK_AMD_negative_viewport_height", 347, 1},
  {"VK_AMD_shader_image_load_store_lod", 348, 1},
  {"VK_NV_external_memory", 349, 2},
  {"VK_NV_clip_space_w_scaling", 351, 1},
  {"VK_EXT_direct_mode_display", 352, 1},
  {"VK_EXT_display_surface_counter", 353, 1},
  {"VK_EXT_display_control", 354, 4},
  {"VK_GOOGLE_display_timing", 358, 2},
  {"VK_NV_sample_mask_override_coverage", 360, 1},
  {"VK_EXT_conservative_rasterization", 361, 1},
  {"VK_EXT_external_memory_dma_buf", 362, 11},
  {"VK_EXT_sampler_filter_minmax", 373, 2},
  {"VK_EXT_blend_operation_advanced", 375, 1},
  {"VK_EXT_validation_cache", 376, 4},
  {"VK_EXT_descriptor_indexing", 380, 3},
  {"VK_NV_ray_tracing", 383, 13},
  {"VK_NV_representative_fragment_test", 396, 1},
  {"VK_AMD_buffer_marker", 397, 1},
  {"VK_AMD_pipeline_compiler_control", 398, 2},
  {"VK_AMD_shader_core_properties", 400, 3},
  {"VK_NV_fragment_shader_barycentric", 403, 1},
  {"VK_NV_device_diagnostic_checkpoints", 404, 2},
  {"VK_INTEL_shader_integer_functions2", 406, 9},
  {"VK_EXT_pci_bus_info", 415, 1},
  {"VK_EXT_fragment_density_map", 416, 1},
  {"VK_EXT_tooling_info", 417, 1},
  {"VK_EXT_separate_stencil_usage", 418, 1},
  {"VK_NV_coverage_reduction_mode", 419, 1},
  {"VK_EXT_fragment_shader_interlock", 420, 1},
  {"VK_EXT_line_rasterization", 421, 1},
  {"VK_EXT_shader_atomic_float", 422, 1},
  {"VK_EXT_index_type_uint8", 423, 12},
  {"VK_EXT_shader_atomic_float2", 435, 6},
  {"VK_NV_inherited_viewport_scissor", 441, 2},
  {"VK_EXT_robustness2", 443, 4},
  {"VK_EXT_pipeline_creation_cache_control", 447, 1},
  {"VK_NV_ray_tracing_motion_blur", 448, 2},
  {"VK_VALVE_mutable_descriptor_type", 450, 1},
  {"VK_EXT_physical_device_drm", 451, 2},
  {"VK_HUAWEI_invocation_mask", 453, 1},
  {"VK_NV_external_memory_rdma", 454, 1},
  {"VK_EXT_extended_dynamic_state2", 455, 5},
  {"VK_EXT_color_write_enable", 460, 1},
  {"VK_EXT_primitives_generated_query", 461, 2},
  {"VK_EXT_image_2d_view_of_3d", 463, 1},
  {"VK_VALVE_descriptor_set_host_mapping", 464, 2},
  {"VK_QCOM_fragment_density_map_offset", 466, 16},
  {"VK_KHR_ray_tracing_pipeline", 482, 6},
  {"VK_KHR_win32_surface", 488, 2},
  {"VK_KHR_external_memory_win32", 490, 2},
  {"VK_KHR_win32_keyed_mutex", 492, 2},
  {"VK_KHR_external_fence_win32", 494, 2},
  {"VK_NV_external_memory_win32", 496, 1},
  {"VK_NV_win32_keyed_mutex", 497, 4}
};

 constexpr const char* const function_names[] = {
  "vkGetInstanceProcAddr",
  "vkEnumerateInstanceVersion",
  "vkEnumerateInstanceExtensionProperties",
  "vkEnumerateInstanceLayerProperties",
  "vkCreateInstance",
  "vkDestroyInstance",
  "vkEnumeratePhysicalDevices",
  "vkGetPhysicalDeviceFeatures",
  "vkGetPhysicalDeviceFormatProperties",
  "vkGetPhysicalDeviceImageFormatProperties",
  "vkGetPhysicalDeviceProperties",
  "vkGetPhysicalDeviceQueueFamilyProperties",
  "vkGetPhysicalDeviceMemoryProperties",
  "vkGetDeviceProcAddr",
  "vkCreateDevice",
  "vkDestroyDevice",
  "vkEnumerateDeviceExtensionProperties",
  "vkEnumerateDeviceLayerProperties",
  "vkGetDeviceQueue",
  "vkQueueSubmit",
  "vkQueueWaitIdle",
  "vkDeviceWaitIdle",
  "vkAllocateMemory",
  "vkFreeMemory",
  "vkMapMemory",
  "vkUnmapMemory",
  "vkFlushMappedMemoryRanges",
  "vkInvalidateMappedMemoryRanges",
  "vkGetDeviceMemoryCommitment",
  "vkBindBufferMemory",
  "vkBindImageMemory",
  "vkGetBufferMemoryRequirements",
  "vkGetImageMemoryRequirements",
  "vkGetImageSparseMemoryRequirements",
  "vkGetPhysicalDeviceSparseImageFormatProperties",
  "vkQueueBindSparse",
  "vkCreateFence",
  "vkDestroyFence",
  "vkResetFences",
  "vkGetFenceStatus",
  "vkWaitForFences",
  "vkCreateSemaphore",
  "vkDestroySemaphore",
  "vkCreateEvent",
  "vkDestroyEvent",
  "vkGetEventStatus",
  "vkSetEvent",
  "vkResetEvent",
  "vkCreateQueryPool",
  "vkDestroyQueryPool",
  "vkGetQueryPoolResults",
  "vkCreateBuffer",
  "vkDestroyBuffer",
  "vkCreateBufferView",
  "vkDestroyBufferView",
  "vkCreateImage",
  "vkDestroyImage",
  "vkGetImageSubresourceLayout",
  "vkCreateImageView",
  "vkDestroyImageView",
  "vkCreateShaderModule",
  "vkDestroyShaderModule",
  "vkCreatePipelineCache",
  "vkDestroyPipelineCache",
  "vkGetPipelineCacheData",
  "vkMergePipelineCaches",
  "vkCreateGraphicsPipelines",
  "vkCreateComputePipelines",
  "vkDestroyPipeline",
  "vkCreatePipelineLayout",
  "vkDestroyPipelineLayout",
  "vkCreateSampler",
  "vkDestroySampler",
  "vkCreateDescriptorSetLayout",
  "vkDestroyDescriptorSetLayout",
  "vkCreateDescriptorPool",
  "vkDestroyDescriptorPool",
  "vkResetDescriptorPool",
  "vkAllocateDescriptorSets",
  "vkFreeDescriptorSets",
  "vkUpdateDescriptorSets",
  "vkCreateFramebuffer",
  "vkDestroyFramebuffer",
  "vkCreateRenderPass",
  "vkDestroyRenderPass",
  "vkGetRenderAreaGranularity",
  "vkCreateCommandPool",
  "vkDestroyCommandPool",
  "vkResetCommandPool",
  "vkAllocateCommandBuffers",
  "vkFreeCommandBuffers",
  "vkBeginCommandBuffer",
  "vkEndCommandBuffer",
  "vkResetCommandBuffer",
  "vkCmdBindPipeline",
  "vkCmdSetViewport",
  "vkCmdSetScissor",
  "vkCmdSetLineWidth",
  "vkCmdSetDepthBias",
  "vkCmdSetBlendConstants",
  "vkCmdSetDepthBounds",
  "vkCmdSetStencilCompareMask",
  "vkCmdSetStencilWriteMask",
  "vkCmdSetStencilReference",
  "vkCmdBindDescriptorSets",
  "vkCmdBindIndexBuffer",
  "vkCmdBindVertexBuffers",
  "vkCmdDraw",
  "vkCmdDrawIndexed",
  "vkCmdDrawIndirect",
  "vkCmdDrawIndexedIndirect",
  "vkCmdDispatch",
  "vkCmdDispatchIndirect",
  "vkCmdCopyBuffer",
  "vkCmdCopyImage",
  "vkCmdBlitImage",
  "vkCmdCopyBufferToImage",
  "vkCmdCopyImageToBuffer",
  "vkCmdUpdateBuffer",
  "vkCmdFillBuffer",
  "vkCmdClearColorImage",
  "vkCmdClearDepthStencilImage",
  "vkCmdClearAttachments",
  "vkCmdResolveImage",
  "vkCmdSetEvent",
  "vkCmdResetEvent",
  "vkCmdWaitEvents",
  "vkCmdPipelineBarrier",
  "vkCmdBeginQuery",
  "vkCmdEndQuery",
  "vkCmdResetQueryPool",
  "vkCmdWriteTimestamp",
  "vkCmdCopyQueryPoolResults",
  "vkCmdPushConstants",
  "vkCmdBeginRenderPass",
  "vkCmdNextSubpass",
  "vkCmdEndRenderPass",
  "vkCmdExecuteCommands",
  "vkBindBufferMemory2",
  "vkBindImageMemory2",
  "vkGetDeviceGroupPeerMemoryFeatures",
  "vkCmdSetDeviceMask",
  "vkCmdDispatchBase",
  "vkEnumeratePhysicalDeviceGroups",
  "vkGetImageMemoryRequirements2",
  "vkGetBufferMemoryRequirements2",
  "vkGetImageSparseMemoryRequirements2",
  "vkGetPhysicalDeviceFeatures2",
  "vkGetPhysicalDeviceProperties2",
  "vkGetPhysicalDeviceFormatProperties2",
  "vkGetPhysicalDeviceImageFormatProperties2",
  "vkGetPhysicalDeviceQueueFamilyProperties2",
  "vkGetPhysicalDeviceMemoryProperties2",
  "vkGetPhysicalDeviceSparseImageFormatProperties2",
  "vkTrimCommandPool",
  "vkGetDeviceQueue2",
  "vkCreateSamplerYcbcrConversion",
  "vkDestroySamplerYcbcrConversion",
  "vkCreateDescriptorUpdateTemplate",
  "vkDestroyDescriptorUpdateTemplate",
  "vkUpdateDescriptorSetWithTemplate",
  "vkGetPhysicalDeviceExternalBufferProperties",
  "vkGetPhysicalDeviceExternalFenceProperties",
  "vkGetPhysicalDeviceExternalSemaphoreProperties",
  "vkGetDescriptorSetLayoutSupport",
  "vkCmdDrawIndirectCount",
  "vkCmdDrawIndexedIndirectCount",
  "vkCreateRenderPass2",
  "vkCmdBeginRenderPass2",
  "vkCmdNextSubpass2",
  "vkCmdEndRenderPass2",
  "vkResetQueryPool",
  "vkGetSemaphoreCounterValue",
  "vkWaitSemaphores",
  "vkSignalSemaphore",
  "vkGetBufferDeviceAddress",
  "vkGetBufferOpaqueCaptureAddress",
  "vkGetDeviceMemoryOpaqueCaptureAddress",
  "vkGetPhysicalDeviceToolProperties",
  "vkCreatePrivateDataSlot",
  "vkDestroyPrivateDataSlot",
  "vkSetPrivateData",
  "vkGetPrivateData",
  "vkCmdSetEvent2",
  "vkCmdResetEvent2",
  "vkCmdWaitEvents2",
  "vkCmdPipelineBarrier2",
  "vkCmdWriteTimestamp2",
  "vkQueueSubmit2",
  "vkCmdCopyBuffer2",
  "vkCmdCopyImage2",
  "vkCmdCopyBufferToImage2",
  "vkCmdCopyImageToBuffer2",
  "vkCmdBlitImage2",
  "vkCmdResolveImage2",
  "vkCmdBeginRendering",
  "vkCmdEndRendering",
  "vkCmdSetCullMode",
  "vkCmdSetFrontFace",
  "vkCmdSetPrimitiveTopology",
  "vkCmdSetViewportWithCount",
  "vkCmdSetScissorWithCount",
  "vkCmdBindVertexBuffers2",
  "vkCmdSetDepthTestEnable",
  "vkCmdSetDepthWriteEnable",
  "vkCmdSetDepthCompareOp",
  "vkCmdSetDepthBoundsTestEnable",
  "vkCmdSetStencilTestEnable",
  "vkCmdSetStencilOp",
  "vkCmdSetRasterizerDiscardEnable",
  "vkCmdSetDepthBiasEnable",
  "vkCmdSetPrimitiveRestartEnable",
  "vkGetDeviceBufferMemoryRequirements",
  "vkGetDeviceImageMemoryRequirements",
  "vkGetDeviceImageSparseMemoryRequirements",
  "vkDestroySurfaceKHR",
  "vkGetPhysicalDeviceSurfaceSupportKHR",
  "vkGetPhysicalDeviceSurfaceCapabilitiesKHR",
  "vkGetPhysicalDeviceSurfaceFormatsKHR",
  "vkGetPhysicalDeviceSurfacePresentModesKHR",
  "vkCreateSwapchainKHR",
  "vkDestroySwapchainKHR",
  "vkGetSwapchainImagesKHR",
  "vkAcquireNextImageKHR",
  "vkQueuePresentKHR",
  "vkGetDeviceGroupPresentCapabilitiesKHR",
  "vkGetDeviceGroupSurfacePresentModesKHR",
  "vkGetPhysicalDevicePresentRectanglesKHR",
  "vkAcquireNextImage2KHR",
  "vkGetPhysicalDeviceDisplayPropertiesKHR",
  "vkGetPhysicalDeviceDisplayPlanePropertiesKHR",
  "vkGetDisplayPlaneSupportedDisplaysKHR",
  "vkGetDisplayModePropertiesKHR",
  "vkCreateDisplayModeKHR",
  "vkGetDisplayPlaneCapabilitiesKHR",
  "vkCreateDisplayPlaneSurfaceKHR",
  "vkCreateSharedSwapchainsKHR",
  "vkCmdBeginRenderingKHR",
  "vkCmdEndRenderingKHR",
  "vkGetPhysicalDeviceFeatures2KHR",
  "vkGetPhysicalDeviceProperties2KHR",
  "vkGetPhysicalDeviceFormatProperties2KHR",
  "vkGetPhysicalDeviceImageFormatProperties2KHR",
  "vkGetPhysicalDeviceQueueFamilyProperties2KHR",
  "vkGetPhysicalDeviceMemoryProperties2KHR",
  "vkGetPhysicalDeviceSparseImageFormatProperties2KHR",
  "vkGetDeviceGroupPeerMemoryFeaturesKHR",
  "vkCmdSetDeviceMaskKHR",
  "vkCmdDispatchBaseKHR",
  "vkTrimCommandPoolKHR",
  "vkEnumeratePhysicalDeviceGroupsKHR",
  "vkGetPhysicalDeviceExternalBufferPropertiesKHR",
  "vkGetMemoryFdKHR",
  "vkGetMemoryFdPropertiesKHR",
  "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR",
  "vkImportSemaphoreFdKHR",
  "vkGetSemaphoreFdKHR",
  "vkCmdPushDescriptorSetKHR",
  "vkCmdPushDescriptorSetWithTemplateKHR",
  "vkCreateDescriptorUpdateTemplateKHR",
  "vkDestroyDescriptorUpdateTemplateKHR",
  "vkUpdateDescriptorSetWithTemplateKHR",
  "vkCreateRenderPass2KHR",
  "vkCmdBeginRenderPass2KHR",
  "vkCmdNextSubpass2KHR",
  "vkCmdEndRenderPass2KHR",
  "vkGetSwapchainStatusKHR",
  "vkGetPhysicalDeviceExternalFencePropertiesKHR",
  "vkImportFenceFdKHR",
  "vkGetFenceFdKHR",
  "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR",
  "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR",
  "vkAcquireProfilingLockKHR",
  "vkReleaseProfilingLockKHR",
  "vkGetPhysicalDeviceSurfaceCapabilities2KHR",
  "vkGetPhysicalDeviceSurfaceFormats2KHR",
  "vkGetPhysicalDeviceDisplayProperties2KHR",
  "vkGetPhysicalDeviceDisplayPlaneProperties2KHR",
  "vkGetDisplayModeProperties2KHR",
  "vkGetDisplayPlaneCapabilities2KHR",
  "vkGetImageMemoryRequirements2KHR",
  "vkGetBufferMemoryRequirements2KHR",
  "vkGetImageSparseMemoryRequirements2KHR",
  "vkCreateSamplerYcbcrConversionKHR",
  "vkDestroySamplerYcbcrConversionKHR",
  "vkBindBufferMemory2KHR",
  "vkBindImageMemory2KHR",
  "vkGetDescriptorSetLayoutSupportKHR",
  "vkCmdDrawIndirectCountKHR",
  "vkCmdDrawIndexedIndirectCountKHR",
  "vkGetSemaphoreCounterValueKHR",
  "vkWaitSemaphoresKHR",
  "vkSignalSemaphoreKHR",
  "vkGetPhysicalDeviceFragmentShadingRatesKHR",
  "vkCmdSetFragmentShadingRateKHR",
  "vkWaitForPresentKHR",
  "vkGetBufferDeviceAddressKHR",
  "vkGetBufferOpaqueCaptureAddressKHR",
  "vkGetDeviceMemoryOpaqueCaptureAddressKHR",
  "vkCreateDeferredOperationKHR",
  "vkDestroyDeferredOperationKHR",
  "vkGetDeferredOperationMaxConcurrencyKHR",
  "vkGetDeferredOperationResultKHR",
  "vkDeferredOperationJoinKHR",
  "vkGetPipelineExecutablePropertiesKHR",
  "vkGetPipelineExecutableStatisticsKHR",
  "vkGetPipelineExecutableInternalRepresentationsKHR",
  "vkCmdSetEvent2KHR",
  "vkCmdResetEvent2KHR",
  "vkCmdWaitEvents2KHR",
  "vkCmdPipelineBarrier2KHR",
  "vkCmdWriteTimestamp2KHR",
  "vkQueueSubmit2KHR",
  "vkCmdWriteBufferMarker2AMD",
  "vkGetQueueCheckpointData2NV",
  "vkCmdCopyBuffer2KHR",
  "vkCmdCopyImage2KHR",
  "vkCmdCopyBufferToImage2KHR",
  "vkCmdCopyImageToBuffer2KHR",
  "vkCmdBlitImage2KHR",
  "vkCmdResolveImage2KHR",
  "vkGetDeviceBufferMemoryRequirementsKHR",
  "vkGetDeviceImageMemoryRequirementsKHR",
  "vkGetDeviceImageSparseMemoryRequirementsKHR",
  "vkCreateDebugReportCallbackEXT",
  "vkDestroyDebugReportCallbackEXT",
  "vkDebugReportMessageEXT",
  "vkDebugMarkerSetObjectTagEXT",
  "vkDebugMarkerSetObjectNameEXT",
  "vkCmdDebugMarkerBeginEXT",
  "vkCmdDebugMarkerEndEXT",
  "vkCmdDebugMarkerInsertEXT",
  "vkCmdBindTransformFeedbackBuffersEXT",
  "vkCmdBeginTransformFeedbackEXT",
  "vkCmdEndTransformFeedbackEXT",
  "vkCmdBeginQueryIndexedEXT",
  "vkCmdEndQueryIndexedEXT",
  "vkCmdDrawIndirectByteCountEXT",
  "vkCreateCuModuleNVX",
  "vkCreateCuFunctionNVX",
  "vkDestroyCuModuleNVX",
  "vkDestroyCuFunctionNVX",
  "vkCmdCuLaunchKernelNVX",
  "vkGetImageViewHandleNVX",
  "vkGetImageViewAddressNVX",
  "vkCmdDrawIndirectCountAMD",
  "vkCmdDrawIndexedIndirectCountAMD",
  "vkGetShaderInfoAMD",
  "vkGetPhysicalDeviceExternalImageFormatPropertiesNV",
  "vkCmdBeginConditionalRenderingEXT",
  "vkCmdEndConditionalRenderingEXT",
  "vkCmdSetViewportWScalingNV",
  "vkReleaseDisplayEXT",
  "vkGetPhysicalDeviceSurfaceCapabilities2EXT",
  "vkDisplayPowerControlEXT",
  "vkRegisterDeviceEventEXT",
  "vkRegisterDisplayEventEXT",
  "vkGetSwapchainCounterEXT",
  "vkGetRefreshCycleDurationGOOGLE",
  "vkGetPastPresentationTimingGOOGLE",
  "vkCmdSetDiscardRectangleEXT",
  "vkSetHdrMetadataEXT",
  "vkSetDebugUtilsObjectNameEXT",
  "vkSetDebugUtilsObjectTagEXT",
  "vkQueueBeginDebugUtilsLabelEXT",
  "vkQueueEndDebugUtilsLabelEXT",
  "vkQueueInsertDebugUtilsLabelEXT",
  "vkCmdBeginDebugUtilsLabelEXT",
  "vkCmdEndDebugUtilsLabelEXT",
  "vkCmdInsertDebugUtilsLabelEXT",
  "vkCreateDebugUtilsMessengerEXT",
  "vkDestroyDebugUtilsMessengerEXT",
  "vkSubmitDebugUtilsMessageEXT",
  "vkCmdSetSampleLocationsEXT",
  "vkGetPhysicalDeviceMultisamplePropertiesEXT",
  "vkGetImageDrmFormatModifierPropertiesEXT",
  "vkCreateValidationCacheEXT",
  "vkDestroyValidationCacheEXT",
  "vkMergeValidationCachesEXT",
  "vkGetValidationCacheDataEXT",
  "vkCmdBindShadingRateImageNV",
  "vkCmdSetViewportShadingRatePaletteNV",
  "vkCmdSetCoarseSampleOrderNV",
  "vkCreateAccelerationStructureNV",
  "vkDestroyAccelerationStructureNV",
  "vkGetAccelerationStructureMemoryRequirementsNV",
  "vkBindAccelerationStructureMemoryNV",
  "vkCmdBuildAccelerationStructureNV",
  "vkCmdCopyAccelerationStructureNV",
  "vkCmdTraceRaysNV",
  "vkCreateRayTracingPipelinesNV",
  "vkGetRayTracingShaderGroupHandlesKHR",
  "vkGetRayTracingShaderGroupHandlesNV",
  "vkGetAccelerationStructureHandleNV",
  "vkCmdWriteAccelerationStructuresPropertiesNV",
  "vkCompileDeferredNV",
  "vkGetMemoryHostPointerPropertiesEXT",
  "vkCmdWriteBufferMarkerAMD",
  "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT",
  "vkGetCalibratedTimestampsEXT",
  "vkCmdDrawMeshTasksNV",
  "vkCmdDrawMeshTasksIndirectNV",
  "vkCmdDrawMeshTasksIndirectCountNV",
  "vkCmdSetExclusiveScissorNV",
  "vkCmdSetCheckpointNV",
  "vkGetQueueCheckpointDataNV",
  "vkInitializePerformanceApiINTEL",
  "vkUninitializePerformanceApiINTEL",
  "vkCmdSetPerformanceMarkerINTEL",
  "vkCmdSetPerformanceStreamMarkerINTEL",
  "vkCmdSetPerformanceOverrideINTEL",
  "vkAcquirePerformanceConfigurationINTEL",
  "vkReleasePerformanceConfigurationINTEL",
  "vkQueueSetPerformanceConfigurationINTEL",
  "vkGetPerformanceParameterINTEL",
  "vkSetLocalDimmingAMD",
  "vkGetBufferDeviceAddressEXT",
  "vkGetPhysicalDeviceToolPropertiesEXT",
  "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV",
  "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV",
  "vkCreateHeadlessSurfaceEXT",
  "vkCmdSetLineStippleEXT",
  "vkResetQueryPoolEXT",
  "vkCmdSetCullModeEXT",
  "vkCmdSetFrontFaceEXT",
  "vkCmdSetPrimitiveTopologyEXT",
  "vkCmdSetViewportWithCountEXT",
  "vkCmdSetScissorWithCountEXT",
  "vkCmdBindVertexBuffers2EXT",
  "vkCmdSetDepthTestEnableEXT",
  "vkCmdSetDepthWriteEnableEXT",
  "vkCmdSetDepthCompareOpEXT",
  "vkCmdSetDepthBoundsTestEnableEXT",
  "vkCmdSetStencilTestEnableEXT",
  "vkCmdSetStencilOpEXT",
  "vkGetGeneratedCommandsMemoryRequirementsNV",
  "vkCmdPreprocessGeneratedCommandsNV",
  "vkCmdExecuteGeneratedCommandsNV",
  "vkCmdBindPipelineShaderGroupNV",
  "vkCreateIndirectCommandsLayoutNV",
  "vkDestroyIndirectCommandsLayoutNV",
  "vkAcquireDrmDisplayEXT",
  "vkGetDrmDisplayEXT",
  "vkCreatePrivateDataSlotEXT",
  "vkDestroyPrivateDataSlotEXT",
  "vkSetPrivateDataEXT",
  "vkGetPrivateDataEXT",
  "vkCmdSetFragmentShadingRateEnumNV",
  "vkAcquireWinrtDisplayNV",
  "vkGetWinrtDisplayNV",
  "vkCmdSetVertexInputEXT",
  "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI",
  "vkCmdSubpassShadingHUAWEI",
  "vkCmdBindInvocationMaskHUAWEI",
  "vkGetMemoryRemoteAddressNV",
  "vkCmdSetPatchControlPointsEXT",
  "vkCmdSetRasterizerDiscardEnableEXT",
  "vkCmdSetDepthBiasEnableEXT",
  "vkCmdSetLogicOpEXT",
  "vkCmdSetPrimitiveRestartEnableEXT",
  "vkCmdSetColorWriteEnableEXT",
  "vkCmdDrawMultiEXT",
  "vkCmdDrawMultiIndexedEXT",
  "vkSetDeviceMemoryPriorityEXT",
  "vkGetDescriptorSetLayoutHostMappingInfoVALVE",
  "vkGetDescriptorSetHostMappingVALVE",
  "vkCreateAccelerationStructureKHR",
  "vkDestroyAccelerationStructureKHR",
  "vkCmdBuildAccelerationStructuresKHR",
  "vkCmdBuildAccelerationStructuresIndirectKHR",
  "vkBuildAccelerationStructuresKHR",
  "vkCopyAccelerationStructureKHR",
  "vkCopyAccelerationStructureToMemoryKHR",
  "vkCopyMemoryToAccelerationStructureKHR",
  "vkWriteAccelerationStructuresPropertiesKHR",
  "vkCmdCopyAccelerationStructureKHR",
  "vkCmdCopyAccelerationStructureToMemoryKHR",
  "vkCmdCopyMemoryToAccelerationStructureKHR",
  "vkGetAccelerationStructureDeviceAddressKHR",
  "vkCmdWriteAccelerationStructuresPropertiesKHR",
  "vkGetDeviceAccelerationStructureCompatibilityKHR",
  "vkGetAccelerationStructureBuildSizesKHR",
  "vkCmdTraceRaysKHR",
  "vkCreateRayTracingPipelinesKHR",
  "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR",
  "vkCmdTraceRaysIndirectKHR",
  "vkGetRayTracingShaderGroupStackSizeKHR",
  "vkCmdSetRayTracingPipelineStackSizeKHR",
  "vkCreateWin32SurfaceKHR",
  "vkGetPhysicalDeviceWin32PresentationSupportKHR",
  "vkGetMemoryWin32HandleKHR",
  "vkGetMemoryWin32HandlePropertiesKHR",
  "vkImportSemaphoreWin32HandleKHR",
  "vkGetSemaphoreWin32HandleKHR",
  "vkImportFenceWin32HandleKHR",
  "vkGetFenceWin32HandleKHR",
  "vkGetMemoryWin32HandleNV",
  "vkGetPhysicalDeviceSurfacePresentModes2EXT",
  "vkAcquireFullScreenExclusiveModeEXT",
  "vkReleaseFullScreenExclusiveModeEXT",
  "vkGetDeviceGroupSurfacePresentModes2EXT"
};

} // namespace

std::pair<FunctionGroupStartIndex, FunctionGroupCount> GetFunctionGroupLoadInfo(const std::string& group_name) noexcept
{
    for (const auto& [name, offset, size] : offset_list)
    {
        if (group_name == name)
        {
            return {offset, size};
        }
    }
    return {0, 0};
}

const char* GetFunctionName(uint32_t index) noexcept
{
    if (index >= 501)
    {
        LOG_ERROR("Invalid function index: " + std::to_string(index));
        return nullptr;
    }
    return function_names[index];
};

// clang-format on    

} // namespace plex::graphics::vkapi::loader
