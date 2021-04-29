#include "VulkanUtils.h"
#pragma comment(lib,"vulkan-1.lib")
#ifndef vkDestroyDebugReportCallback
#define vkDestroyDebugReportCallback vkDestroyDebugReportCallbackEXT
#define PFN_vkDestroyDebugReportCallback PFN_vkDestroyDebugReportCallbackEXT
#endif
struct BufferDescription {
	VkImage mImage;
	VkDeviceMemory mMemory;
	VkImageView mImageView;
	VkSampler mSampler;
	VkImageLayout mInitLayout;
	VkImageLayout mTargetLayout;
	VkPipelineStageFlags mSrcStage;
	VkPipelineStageFlags mTargetStage;
	VkImageAspectFlags mImageAspectFlag;
	VkFormat mFormat;
	VkFilter mMinFilter, mMagFilter;
	VkSamplerAddressMode mWrapU, mWrapV, mWrapW;
	VkBool32 mbEnableAnisotropy;
	float mMaxAnisotropy;
	BufferDescription(VkImageAspectFlags image_aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	~BufferDescription();
};
static HWND sHWND=nullptr;
static VkInstance sVulkanInstance = nullptr;
static VkSurfaceKHR sVulkanSurface;
static VkDebugReportCallbackEXT sVulkanDebugReporter;
static VkDevice sVulkanLogicDevice;
static VkQueue sGraphicQueue, sPresentQueue;
static VkPhysicalDevice sVulkanPhysicalDevice,*sAvaliablePhysicalDevices=nullptr;
static uint32_t sAvaliablePhysicalDeviceCount = 0;
static VkSampleCountFlagBits sMaxBufferSampleCount = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
static VkSampleCountFlagBits sDefaultFrameBufferSampleCount = VK_SAMPLE_COUNT_1_BIT;
static int sGraphicQueueFamily, sPresentQueueFamily;
static uint32_t sViewportWidth = 0, sViewportHeight = 0;
static VkFormat sSwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
static VkSwapchainKHR sSwapchain;
static std::vector<VkImage> sSwapchainImages;
static std::vector<VkImageView> sSwapchainImageViews;
static bool sNeedRebuiltDefaultFrameBuffer = true;
static VkCommandPool sCommandPool;
static VkSemaphore sReadyToRender, sReadyToPresent;
static VkRenderPass sDefaultRenderPass = 0;
static bool sNeedRebuiltDefaultRenderPass = true;
static VkFramebuffer *sDefaultFrameBuffers = nullptr;
static TextureObject* sDefaultColorBuffer = nullptr,*sDefaultDepthStencilBuffer=nullptr;
static uint32_t sCurrentDefaultRT = 0;
static VkCommandBuffer sMainCommandBuffer=nullptr;
#define FUNCTION_NAME(prefix,name) prefix##name
#define STATIC_FUNCTION_VAR(prefix,name) FUNCTION_NAME(s##prefix,name)
#define STATIC_VULKAN_INSTANCE_FUNCTION_NAME(name) STATIC_FUNCTION_VAR(VulkanInstanceFunction_,name)
#define DEFINE_STATIC_VULKAN_INSTANCE_FUNCTION(FunctionType) static PFN_##FunctionType STATIC_VULKAN_INSTANCE_FUNCTION_NAME(FunctionType) = nullptr
#define GET_STATIC_VULKAN_INSTANCE_FUNCTION(VulkanInstance,FunctionType) \
	STATIC_VULKAN_INSTANCE_FUNCTION_NAME(FunctionType) = (PFN_##FunctionType)vkGetInstanceProcAddr(VulkanInstance,#FunctionType)
#define GET_STATIC_VULKAN_INSTANCE_FUNCTION_SPECIFY_PROCESS_FUNCTION_NAME(VulkanInstance,FunctionType,FunctionName) \
	STATIC_VULKAN_INSTANCE_FUNCTION_NAME(FunctionType) = (PFN_##FunctionType)vkGetInstanceProcAddr(VulkanInstance,#FunctionName)

#define STATIC_VULKAN_DEVICE_FUNCTION_NAME(name) STATIC_FUNCTION_VAR(VulkanDeviceFunction_,name)
#define DEFINE_STATIC_VULKAN_DEVICE_FUNCTION(FunctionType) static PFN_##FunctionType STATIC_VULKAN_DEVICE_FUNCTION_NAME(FunctionType) = nullptr
#define GET_STATIC_VULKAN_DEVICE_FUNCTION(VulkanDevice,FunctionType) \
	STATIC_VULKAN_DEVICE_FUNCTION_NAME(FunctionType) = (PFN_##FunctionType)vkGetDeviceProcAddr(VulkanDevice,#FunctionType)

DEFINE_STATIC_VULKAN_INSTANCE_FUNCTION(vkCreateDebugReportCallbackEXT);
DEFINE_STATIC_VULKAN_INSTANCE_FUNCTION(vkDestroyDebugReportCallback);
DEFINE_STATIC_VULKAN_INSTANCE_FUNCTION(vkCreateWin32SurfaceKHR);
DEFINE_STATIC_VULKAN_DEVICE_FUNCTION(vkCmdDebugMarkerBeginEXT);
DEFINE_STATIC_VULKAN_DEVICE_FUNCTION(vkCmdDebugMarkerEndEXT);
DEFINE_STATIC_VULKAN_DEVICE_FUNCTION(vkCmdDebugMarkerInsertEXT);


static void InitVulkanInstance() {
	VkApplicationInfo appInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO,
		nullptr,
		"VulkanStady",
		VK_MAKE_VERSION(1, 0, 0),
		"Alice",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_1
	};
	const char* enabledExtensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};
	const char* enabledLayers[] = {
		"VK_LAYER_KHRONOS_validation",
		"VK_LAYER_RENDERDOC_Capture"
	};
	VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &appInfo;
	instanceCreateInfo.enabledExtensionCount = RAW_C_ARRAY_ELEMENT_COUNT(enabledExtensions);
	instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions;
	instanceCreateInfo.enabledLayerCount = RAW_C_ARRAY_ELEMENT_COUNT(enabledLayers);
	instanceCreateInfo.ppEnabledLayerNames = enabledLayers;
	vkCreateInstance(&instanceCreateInfo, nullptr, &sVulkanInstance);
	GET_STATIC_VULKAN_INSTANCE_FUNCTION(sVulkanInstance, vkCreateDebugReportCallbackEXT);
	GET_STATIC_VULKAN_INSTANCE_FUNCTION(sVulkanInstance, vkDestroyDebugReportCallback);
	GET_STATIC_VULKAN_INSTANCE_FUNCTION(sVulkanInstance, vkCreateWin32SurfaceKHR);
}
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReporterCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	uint64_t                    object,
	size_t                      location,
	int32_t                     messageCode,
	const char*					pLayerPrefix,
	const char*					pMessage,
	void*						pUserData){
	printf("Validation Layer : %s\n", pMessage);
	return VK_FALSE;
}
static void InitDebugReporter() {
	VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo = {};
	debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	debugReportCallbackCreateInfo.pfnCallback = VulkanDebugReporterCallback;
	STATIC_VULKAN_INSTANCE_FUNCTION_NAME(vkCreateDebugReportCallbackEXT)(sVulkanInstance, &debugReportCallbackCreateInfo, nullptr, &sVulkanDebugReporter);
}
static void InitVulkanSurface() {
	VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
	win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	win32SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	win32SurfaceCreateInfo.hwnd = static_cast<HWND>(sHWND);
	STATIC_VULKAN_INSTANCE_FUNCTION_NAME(vkCreateWin32SurfaceKHR)(sVulkanInstance, &win32SurfaceCreateInfo, nullptr, &sVulkanSurface);
}
VkSampleCountFlagBits GetMaxMSAASampleCount() {
	if (sMaxBufferSampleCount !=VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM){
		return sMaxBufferSampleCount;
	}
	VkPhysicalDeviceProperties physicalproperties;
	vkGetPhysicalDeviceProperties(sVulkanPhysicalDevice, &physicalproperties);
	VkSampleCountFlags max_buffer_sample_count = physicalproperties.limits.framebufferColorSampleCounts >
		physicalproperties.limits.framebufferDepthSampleCounts ?
		physicalproperties.limits.framebufferDepthSampleCounts :
		physicalproperties.limits.framebufferColorSampleCounts;
	if (max_buffer_sample_count & VK_SAMPLE_COUNT_64_BIT) {
		sMaxBufferSampleCount = VK_SAMPLE_COUNT_64_BIT; 
		return sMaxBufferSampleCount;
	}
	if (max_buffer_sample_count & VK_SAMPLE_COUNT_32_BIT) {
		sMaxBufferSampleCount = VK_SAMPLE_COUNT_32_BIT;
		return sMaxBufferSampleCount;
	}
	if (max_buffer_sample_count & VK_SAMPLE_COUNT_16_BIT) { 
		sMaxBufferSampleCount = VK_SAMPLE_COUNT_16_BIT; 
		return sMaxBufferSampleCount;
	}
	if (max_buffer_sample_count & VK_SAMPLE_COUNT_8_BIT) { 
		sMaxBufferSampleCount = VK_SAMPLE_COUNT_8_BIT; 
		return sMaxBufferSampleCount;
	}
	if (max_buffer_sample_count & VK_SAMPLE_COUNT_4_BIT) {
		sMaxBufferSampleCount = VK_SAMPLE_COUNT_4_BIT;
		return sMaxBufferSampleCount;
	}
	if (max_buffer_sample_count & VK_SAMPLE_COUNT_2_BIT) {
		sMaxBufferSampleCount = VK_SAMPLE_COUNT_2_BIT;
		return sMaxBufferSampleCount;
	}
	sMaxBufferSampleCount = VK_SAMPLE_COUNT_1_BIT;
	return sMaxBufferSampleCount;
}
static void InitVulkanPhysicalDevice() {
	vkEnumeratePhysicalDevices(sVulkanInstance, &sAvaliablePhysicalDeviceCount, nullptr);
	if (sAvaliablePhysicalDeviceCount==0){
		return;
	}
	sAvaliablePhysicalDevices = new VkPhysicalDevice[sAvaliablePhysicalDeviceCount];
	vkEnumeratePhysicalDevices(sVulkanInstance, &sAvaliablePhysicalDeviceCount, sAvaliablePhysicalDevices);
}
bool IsVulkanPhysicalDeviceAvaliableFor(VkPhysicalDevice physicalDevice, bool contain_present_queue , bool contain_graphic_queue, bool contain_compute_queue) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties);
	sPresentQueueFamily = -1;
	sGraphicQueueFamily = -1;
	bool ret = false;
	for (uint32_t i = 0; i < queueFamilyCount; ++i) {
		VkBool32 supportPresent = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, sVulkanSurface, &supportPresent);
		if (queueFamilyProperties[i].queueCount > 0) {
			if (supportPresent) {
				sPresentQueueFamily = i;
			}
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				sGraphicQueueFamily = i;
			}
		}
		if (sGraphicQueueFamily != -1 && sPresentQueueFamily != -1) {
			ret = true;
			break;
		}
	}
	delete[] queueFamilyProperties;
	return ret;
}
VkPhysicalDevice SelectPhysicalDevice(bool contain_present_queue = true, bool contain_graphic_queue = true, bool contain_compute_queue = false) {
	if (sAvaliablePhysicalDevices==nullptr){
		return nullptr;
	}
	for (uint32_t i = 0; i < sAvaliablePhysicalDeviceCount; ++i) {
		VkPhysicalDevice current_device = sAvaliablePhysicalDevices[i];
		if (IsVulkanPhysicalDeviceAvaliableFor(current_device,contain_present_queue,contain_graphic_queue,contain_compute_queue)){
			return current_device;
		}
	}
	return nullptr;
}
void InitVulkanLogicDevice() {
	VkDeviceQueueCreateInfo deviceQueueCreateInfo[2];
	int queueCount = 2;
	float priority = 1.0f;
	deviceQueueCreateInfo[0] = {};
	deviceQueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo[0].queueCount = 1;
	deviceQueueCreateInfo[0].queueFamilyIndex = sGraphicQueueFamily;
	deviceQueueCreateInfo[0].pQueuePriorities = &priority;
	deviceQueueCreateInfo[1] = {};
	deviceQueueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	deviceQueueCreateInfo[1].queueCount = 1;
	deviceQueueCreateInfo[1].queueFamilyIndex = sPresentQueueFamily;
	deviceQueueCreateInfo[1].pQueuePriorities = &priority;
	if (sGraphicQueueFamily == sPresentQueueFamily) {
		queueCount = 1;
	}
	const char* enabledExtensionNames[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
	physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
	physicalDeviceFeatures.geometryShader = VK_TRUE;
	physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;
	physicalDeviceFeatures.sampleRateShading = VK_TRUE;

	const char* enabledLayerNames[] = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = queueCount;
	deviceCreateInfo.enabledExtensionCount = RAW_C_ARRAY_ELEMENT_COUNT(enabledExtensionNames);
	deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames;
	deviceCreateInfo.enabledLayerCount = RAW_C_ARRAY_ELEMENT_COUNT(enabledLayerNames);
	deviceCreateInfo.ppEnabledLayerNames = enabledLayerNames;
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
	vkCreateDevice(sVulkanPhysicalDevice, &deviceCreateInfo, nullptr, &sVulkanLogicDevice);
	vkGetDeviceQueue(sVulkanLogicDevice, sGraphicQueueFamily, 0, &sGraphicQueue);
	vkGetDeviceQueue(sVulkanLogicDevice, sPresentQueueFamily, 0, &sPresentQueue);
	GET_STATIC_VULKAN_DEVICE_FUNCTION(sVulkanLogicDevice, vkCmdDebugMarkerBeginEXT);
	GET_STATIC_VULKAN_DEVICE_FUNCTION(sVulkanLogicDevice, vkCmdDebugMarkerEndEXT);
	GET_STATIC_VULKAN_DEVICE_FUNCTION(sVulkanLogicDevice, vkCmdDebugMarkerInsertEXT);
}
static void InitSwapChain() {
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface = sVulkanSurface;
	swapchainCreateInfo.minImageCount = 3;
	swapchainCreateInfo.imageFormat = sSwapchainImageFormat;
	swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent = { sViewportWidth,sViewportHeight };
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	uint32_t queueFamilyIndices[] = { uint32_t(sGraphicQueueFamily),uint32_t(sPresentQueueFamily) };
	if (sGraphicQueueFamily != sPresentQueueFamily) {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
	}
	else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(sVulkanPhysicalDevice, sVulkanSurface, &capabilities);
	swapchainCreateInfo.preTransform = capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	vkCreateSwapchainKHR(sVulkanLogicDevice, &swapchainCreateInfo, nullptr, &sSwapchain);
}
static void InitSwapchainImageViews() {
	uint32_t sSwapchainBufferCount = 0;
	vkGetSwapchainImagesKHR(sVulkanLogicDevice, sSwapchain, &sSwapchainBufferCount, nullptr);
	sSwapchainImages.resize(sSwapchainBufferCount);
	vkGetSwapchainImagesKHR(sVulkanLogicDevice, sSwapchain, &sSwapchainBufferCount, sSwapchainImages.data());
	sSwapchainImageViews.resize(sSwapchainBufferCount);
	for (uint32_t i = 0; i < sSwapchainBufferCount; ++i) {
		GenImageView2D(sSwapchainImageViews[i],sSwapchainImages[i], sSwapchainImageFormat);
	}
}
static void InitDefaultRenderPass() {
	if (sDefaultRenderPass!=0){
		vkDestroyRenderPass(sVulkanLogicDevice, sDefaultRenderPass, nullptr);
		sDefaultRenderPass = 0;
	}
	VkAttachmentDescription colorAttachmentDescription = {
		0,
		sSwapchainImageFormat,
		sDefaultFrameBufferSampleCount,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		sDefaultFrameBufferSampleCount == VK_SAMPLE_COUNT_1_BIT ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkAttachmentDescription depthAttachmentDescription = {
		0,
		VK_FORMAT_D24_UNORM_S8_UINT,
		sDefaultFrameBufferSampleCount,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkAttachmentDescription resolveColorAttachmentDescription = {
		0,
		sSwapchainImageFormat,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	VkAttachmentReference colorAttachmentRef = { 0,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkAttachmentReference depthAttachmentRef = { 1,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
	VkAttachmentReference resolveColorRef = { 2,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkAttachmentDescription attachments[3];
	int attachmentcount = 2;
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	if (sDefaultFrameBufferSampleCount == VK_SAMPLE_COUNT_1_BIT) {
		memcpy(&attachments[0], &colorAttachmentDescription, sizeof(VkAttachmentDescription));
		memcpy(&attachments[1], &depthAttachmentDescription, sizeof(VkAttachmentDescription));
	}
	else {
		subpass.pResolveAttachments = &resolveColorRef;
		attachmentcount = 3;
		memcpy(&attachments[0], &colorAttachmentDescription, sizeof(VkAttachmentDescription));
		memcpy(&attachments[1], &depthAttachmentDescription, sizeof(VkAttachmentDescription));
		memcpy(&attachments[2], &resolveColorAttachmentDescription, sizeof(VkAttachmentDescription));
	}
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = attachmentcount;
	renderPassCreateInfo.pAttachments = attachments;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	vkCreateRenderPass(sVulkanLogicDevice, &renderPassCreateInfo, nullptr, &sDefaultRenderPass);
}
static void InitCommandPool() {
	VkCommandPoolCreateInfo ci = {};
	ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	ci.queueFamilyIndex = sGraphicQueueFamily;
	vkCreateCommandPool(sVulkanLogicDevice, &ci, nullptr, &sCommandPool);
}
static void InitSemaphores() {
	VkSemaphoreCreateInfo ci = {};
	ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(sVulkanLogicDevice, &ci, nullptr, &sReadyToRender);
	vkCreateSemaphore(sVulkanLogicDevice, &ci, nullptr, &sReadyToPresent);
}
static uint32_t RetrieveMemoryType(uint32_t type_filters, VkMemoryPropertyFlags properties){
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(sVulkanPhysicalDevice, &memoryProperties);
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++){
		uint32_t flag = 1 << i;
		if ((type_filters & flag) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties){
			return i;
		}
	}
	return 0;
}
static VkDeviceMemory AllocateGPUMemory(VkDeviceSize size,uint32_t memoryType) {
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = size;
	memoryAllocateInfo.memoryTypeIndex = memoryType;
	VkDeviceMemory memory = 0;
	if(VK_SUCCESS!=vkAllocateMemory(sVulkanLogicDevice, &memoryAllocateInfo, nullptr, &memory)){
		printf("vkAllocateMemory Failed : %llu,%u\n",size,memoryType);
		return 0;
	}
	return memory;
}
static void InitDefaultColorBuffer() {
	if (sDefaultColorBuffer!=nullptr){
		delete sDefaultColorBuffer;
	}
	sDefaultColorBuffer = new TextureObject;
	GenImage(sDefaultColorBuffer->mImage, sViewportWidth, sViewportHeight, sSwapchainImageFormat, sDefaultFrameBufferSampleCount, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
	AllocateImageMemory(sDefaultColorBuffer->mImage, sDefaultColorBuffer->mDeviceMemory);
	GenImageView2D(sDefaultColorBuffer->mImageView,sDefaultColorBuffer->mImage, sSwapchainImageFormat);
}
static void InitDefaultDepthStencilBuffer() {
	if (sDefaultDepthStencilBuffer != nullptr) {
		delete sDefaultDepthStencilBuffer;
	}
	sDefaultDepthStencilBuffer = new TextureObject;
	GenImage(sDefaultDepthStencilBuffer->mImage, sViewportWidth, sViewportHeight, VK_FORMAT_D24_UNORM_S8_UINT, sDefaultFrameBufferSampleCount, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	AllocateImageMemory(sDefaultDepthStencilBuffer->mImage, sDefaultDepthStencilBuffer->mDeviceMemory);
	GenImageView2D(sDefaultDepthStencilBuffer->mImageView, sDefaultDepthStencilBuffer->mImage, VK_FORMAT_D24_UNORM_S8_UINT,0,1,1,0, VK_IMAGE_ASPECT_DEPTH_BIT);
}
static void InitDefaultFrameBuffer() {
	size_t frameBufferCount = sSwapchainImages.size();
	InitDefaultColorBuffer();
	InitDefaultDepthStencilBuffer();
	if (sDefaultFrameBuffers == nullptr) {
		sDefaultFrameBuffers = new VkFramebuffer[frameBufferCount];
	}
	for (int i = 0; i < frameBufferCount; ++i) {
		VkImageView renderTargets[3] = { 0 };
		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		if (sDefaultFrameBufferSampleCount != VK_SAMPLE_COUNT_1_BIT) {
			renderTargets[0] = sDefaultColorBuffer->mImageView;
			renderTargets[1] = sDefaultDepthStencilBuffer->mImageView;
			renderTargets[2] = sSwapchainImageViews[i];
			frameBufferCreateInfo.attachmentCount = 3;
		}
		else {
			renderTargets[0] = sSwapchainImageViews[i];
			renderTargets[1] = sDefaultDepthStencilBuffer->mImageView;
			frameBufferCreateInfo.attachmentCount = 2;
		}
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = sDefaultRenderPass;
		frameBufferCreateInfo.pAttachments = renderTargets;
		frameBufferCreateInfo.width = sViewportWidth;
		frameBufferCreateInfo.height = sViewportHeight;
		frameBufferCreateInfo.layers = 1;
		vkCreateFramebuffer(sVulkanLogicDevice, &frameBufferCreateInfo, nullptr, &sDefaultFrameBuffers[i]);
	}
}
static void RebuiltDefaultFrameBuffers() {
	if (sDefaultColorBuffer != nullptr) {
		delete sDefaultColorBuffer;
		sDefaultColorBuffer = nullptr;
	}
	if (sDefaultDepthStencilBuffer != nullptr) {
		delete sDefaultDepthStencilBuffer;
		sDefaultDepthStencilBuffer = nullptr;
	}
	for (size_t i = 0; i < sSwapchainImages.size(); ++i) {
		vkDestroyFramebuffer(sVulkanLogicDevice, sDefaultFrameBuffers[i], nullptr);
		vkDestroyImageView(sVulkanLogicDevice, sSwapchainImageViews[i], nullptr);
	}
	if (!sSwapchainImageViews.empty()) {
		vkDestroySwapchainKHR(sVulkanLogicDevice, sSwapchain, nullptr);
		sSwapchainImageViews.clear();
	}
	InitSwapChain();
	InitSwapchainImageViews();
	InitDefaultFrameBuffer();
}
void GenCommandBuffer(VkCommandBuffer* commandbuffer, int count, VkCommandBufferLevel level){
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.level = level;
	commandBufferAllocateInfo.commandPool = sCommandPool;
	commandBufferAllocateInfo.commandBufferCount = count;
	if (VK_SUCCESS != vkAllocateCommandBuffers(sVulkanLogicDevice, &commandBufferAllocateInfo, commandbuffer)) {
		printf("vkAllocateCommandBuffers Failed\n");
	}
}
void BeginCommandBuffer(VkCommandBuffer* commandbuffer, VkCommandBufferUsageFlagBits usage){
	GenCommandBuffer(commandbuffer);
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = usage;
	if (VK_SUCCESS != vkBeginCommandBuffer(*commandbuffer, &commandBufferBeginInfo)) {
		printf("vkBeginCommandBuffer Failed\n");
	}
}void WaitForCommmandFinish(VkCommandBuffer commandbuffer)
{
	VkSubmitInfo submitinfo = {};
	submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &commandbuffer;
	VkFence fence;
	VkFenceCreateInfo fenceinfo = {};
	fenceinfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(sVulkanLogicDevice, &fenceinfo, nullptr, &fence);
	vkQueueSubmit(sGraphicQueue, 1, &submitinfo, fence);
	vkWaitForFences(sVulkanLogicDevice, 1, &fence, VK_TRUE, 1000000);
	vkDestroyFence(sVulkanLogicDevice, fence, nullptr);
}
VkFramebuffer AquireRenderTarget() {
	vkAcquireNextImageKHR(sVulkanLogicDevice, sSwapchain, 1000000000, sReadyToRender, VK_NULL_HANDLE, &sCurrentDefaultRT);
	return sDefaultFrameBuffers[sCurrentDefaultRT];
}
void EndRendering(VkCommandBuffer commandBuffer){
	vkCmdEndRenderPass(commandBuffer);
	vkEndCommandBuffer(commandBuffer);
}
static void SubmitDrawCommand(VkCommandBuffer commandBuffer){
	// color attachment 准备好了，可以渲染了
	VkSemaphore read_to_render[] = { sReadyToRender };
	// 准备显示
	VkSemaphore read_to_present[] = { sReadyToPresent };
	VkSubmitInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// 等待一个信号，这个信号表示COLOR_ATTACHMENT_OUTPUT这一步的东西可以被render了
	VkPipelineStageFlags waitstages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = read_to_render;
	info.pWaitDstStageMask = waitstages;
	// 等待信号了开始执行commandbuffer
	info.commandBufferCount = 1;
	info.pCommandBuffers = &commandBuffer;
	// commandbuffer执行完毕了，通知另一个信号量。这个信号量是read_to_present
	info.signalSemaphoreCount = 1;
	info.pSignalSemaphores = read_to_present;
	vkQueueSubmit(sGraphicQueue, 1, &info, VK_NULL_HANDLE);
}
static void PresentBackBuffer(){
	VkSemaphore read_to_present[] = { sReadyToPresent };
	VkSwapchainKHR swapchain = sSwapchain;
	VkPresentInfoKHR present_info = {};
	uint32_t current_render_target_index = sCurrentDefaultRT;
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = read_to_present;
	present_info.pSwapchains = &swapchain;
	present_info.swapchainCount = 1;
	present_info.pImageIndices = &current_render_target_index;
	vkQueuePresentKHR(sPresentQueue, &present_info);
	vkQueueWaitIdle(sPresentQueue);

}
void VulkanSwapBuffers(){
	if (sMainCommandBuffer == nullptr) {
		return;
	}
	EndRendering(sMainCommandBuffer);
	SubmitDrawCommand(sMainCommandBuffer);
	PresentBackBuffer();
	vkFreeCommandBuffers(sVulkanLogicDevice, sCommandPool, 1, &sMainCommandBuffer);
	sMainCommandBuffer = nullptr;
}
TextureObject::~TextureObject() {
	if (mDeviceMemory != 0) {
		vkFreeMemory(sVulkanLogicDevice, mDeviceMemory, nullptr);
	}
	if (mImageView != 0){
		vkDestroyImageView(sVulkanLogicDevice, mImageView, nullptr);
	}
	if (mImage != 0) {
		vkDestroyImage(sVulkanLogicDevice, mImage, nullptr);
	}
}
void GenImage(VkImage& image, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits sample_count, VkImageUsageFlags usage, int mipmap_level_count) {
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent = { width,height,1 };
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.mipLevels = mipmap_level_count;
	imageCreateInfo.format = format;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = usage;
	imageCreateInfo.samples = sample_count;
	if (VK_SUCCESS != vkCreateImage(sVulkanLogicDevice, &imageCreateInfo, nullptr,&image)) {
		printf("GenTextureObject Failed\n");
	}
}
void AllocateImageMemory(VkImage& image, VkDeviceMemory& memory) {
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(sVulkanLogicDevice, image, &memoryRequirements);
	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memoryRequirements.size;
	memory_allocate_info.memoryTypeIndex = RetrieveMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkAllocateMemory(sVulkanLogicDevice, &memory_allocate_info, nullptr, &memory);
	vkBindImageMemory(sVulkanLogicDevice, image, memory, 0);
}
void GenImageView2D(VkImageView & imageView, VkImage image, VkFormat format, uint32_t base_mip_level, uint32_t mipmap_level_count,
		uint32_t layer_count, uint32_t base_layer_index, VkImageAspectFlags imageAspect){
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
	imageViewCreateInfo.subresourceRange.baseMipLevel = base_mip_level;
	imageViewCreateInfo.subresourceRange.levelCount = mipmap_level_count;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = base_layer_index;
	imageViewCreateInfo.subresourceRange.layerCount = layer_count;
	if (VK_SUCCESS != vkCreateImageView(sVulkanLogicDevice, &imageViewCreateInfo, nullptr, &imageView)) {
		printf("GenImageView2D Failed\n");
	}
}
void OnVulkanViewportChanged(uint32_t width, uint32_t height) {
	if (sViewportWidth != width || sViewportHeight != height) {
		sViewportWidth = width;
		sViewportHeight = height;
		sNeedRebuiltDefaultFrameBuffer = true;
	}
}
uint32_t GetViewportWidth() {
	return sViewportWidth;
}
uint32_t GetViewportHeight() {
	return sViewportHeight;
}
void InitVulkan(void*hwnd) {
	sHWND = (HWND)hwnd;
	InitVulkanInstance();
	InitDebugReporter();
	InitVulkanSurface();
	InitVulkanPhysicalDevice();
	sVulkanPhysicalDevice = SelectPhysicalDevice(true, true, false);
	InitVulkanLogicDevice();
	InitCommandPool();
	InitSemaphores();
}
#define UpdateDefaultMSAASampleCount(target_sample_count) \
	do{ \
		if (sDefaultFrameBufferSampleCount != target_sample_count) { \
			sNeedRebuiltDefaultRenderPass = true; \
			sNeedRebuiltDefaultFrameBuffer = true; \
			sDefaultFrameBufferSampleCount = target_sample_count; \
		} \
	}while (0)
void SetMSAASampleCount(int sample_count){
	switch (sample_count){
	case 1:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_1_BIT);
		break;
	case 2:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_2_BIT);
		break;
	case 4:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_4_BIT);
		break;
	case 8:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_8_BIT);
		break;
	case 16:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_16_BIT);
		break;
	case 32:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_32_BIT);
		break;
	case 64:
		UpdateDefaultMSAASampleCount(VK_SAMPLE_COUNT_64_BIT);
		break;
	}
}
VkCommandBuffer BeginRenderPass(VkCommandBuffer commandbuffer){
	VkCommandBuffer cmd;
	if (commandbuffer != nullptr){
		cmd = commandbuffer;
	}else{
		BeginCommandBuffer(&cmd);
	}
	if (sNeedRebuiltDefaultRenderPass){
		sNeedRebuiltDefaultRenderPass = false;
		InitDefaultRenderPass();
	}
	if (sNeedRebuiltDefaultFrameBuffer) {
		sNeedRebuiltDefaultFrameBuffer = false;
		RebuiltDefaultFrameBuffers();
	}
	VkFramebuffer framebuffer = AquireRenderTarget();
	VkRenderPass render_pass = sDefaultRenderPass;
	VkClearValue clear_value[2] = {};
	// 对于颜色缓冲区
	clear_value[0].color = { 0.1f,0.4f,0.6f,1.0f };
	// 深度蒙版缓冲区
	clear_value[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo rpbi = {};
	rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpbi.framebuffer = framebuffer;
	rpbi.renderPass = render_pass;
	rpbi.clearValueCount = 2;
	rpbi.pClearValues = clear_value;
	rpbi.renderArea.extent = { sViewportWidth,sViewportHeight };
	rpbi.renderArea.offset = { 0,0 };
	vkCmdBeginRenderPass(cmd, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
	SetMainCommandBuffer(cmd);
	return  cmd;
}
void SetMainCommandBuffer(VkCommandBuffer commandBuffer) {
	sMainCommandBuffer = commandBuffer;
}
VkCommandBuffer GetMainCommandBuffer() {
	return sMainCommandBuffer;
}
void DeleteGPUBuffer(VkBuffer buffer) {
	vkDestroyBuffer(sVulkanLogicDevice, buffer, nullptr);
}
void FreeGPUMemory(VkDeviceMemory memory) {
	vkFreeMemory(sVulkanLogicDevice, memory, nullptr);
}
VkResult GenBuffer(VkBuffer& buffer, VkDeviceMemory& buffermemory, VkDeviceSize size, VkBufferUsageFlags useFlags, VkMemoryPropertyFlags properties) {
	VkBufferCreateInfo bufferinfo = {};
	bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferinfo.size = size;
	bufferinfo.usage = useFlags;
	VkResult ret = vkCreateBuffer(sVulkanLogicDevice, &bufferinfo, nullptr, &buffer);
	if (ret != VK_SUCCESS){
		printf("faild to create buffer\n");
		return ret;
	}
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(sVulkanLogicDevice, buffer, &requirements);
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = requirements.size;
	memoryAllocateInfo.memoryTypeIndex = RetrieveMemoryType(requirements.memoryTypeBits, properties);
	ret = vkAllocateMemory(sVulkanLogicDevice, &memoryAllocateInfo, nullptr, &buffermemory);
	if (ret != 0){
		printf("faild to allocate memory\n");
		return ret;
	}
	vkBindBufferMemory(sVulkanLogicDevice, buffer, buffermemory, 0);
	return VK_SUCCESS;
}
void BufferSubDataToDeviceMemory(VkBuffer buffer, VkBufferUsageFlags usage, const void* data, VkDeviceSize size) {
	VkBuffer tempBuffer;
	VkDeviceMemory tempMemory;
	GenBuffer(tempBuffer, tempMemory, size, usage, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	BufferSubDataToHostVisiableMemory(tempMemory, data, size);

	VkCommandBuffer commandbuffer;
	BeginCommandBuffer(&commandbuffer);
	VkBufferCopy copy = { 0, 0, size };
	vkCmdCopyBuffer(commandbuffer, tempBuffer, buffer, 1, &copy);
	vkEndCommandBuffer(commandbuffer);
	WaitForCommmandFinish(commandbuffer);
	vkFreeCommandBuffers(sVulkanLogicDevice, sCommandPool, 1, &commandbuffer);

	vkDestroyBuffer(sVulkanLogicDevice, tempBuffer, nullptr);
	vkFreeMemory(sVulkanLogicDevice, tempMemory, nullptr);
}
void BufferSubDataToHostVisiableMemory(VkDeviceMemory& memory,const void* data, int size) {
	void* dst;
	vkMapMemory(sVulkanLogicDevice, memory, 0, size, 0, &dst);
	memcpy(dst, data, size);
	vkUnmapMemory(sVulkanLogicDevice, memory);
}
VkPipelineLayout CreateGraphicPipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo) {
	VkPipelineLayout ret;
	vkCreatePipelineLayout(sVulkanLogicDevice, &pipelineLayoutCreateInfo, nullptr, &ret);
	return ret;
}
VkPipeline CreateGraphicPipeline(VkGraphicsPipelineCreateInfo graphicPipelineCreateInfo) {
	VkPipeline pipeline;
	graphicPipelineCreateInfo.renderPass = sDefaultRenderPass;
	vkCreateGraphicsPipelines(sVulkanLogicDevice, VK_NULL_HANDLE, 1, &graphicPipelineCreateInfo, nullptr, &pipeline);
	return pipeline;
}
VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBinding) {
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayoutCreateInfo  descriptorSetLayoutCreateInfo = {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.bindingCount = uint32_t(descriptorSetLayoutBinding.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayoutBinding.data();
	vkCreateDescriptorSetLayout(sVulkanLogicDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
	return descriptorSetLayout;
}
VkDescriptorPool CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSize, int maxSets) {
	VkDescriptorPool descriptorPool;
	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptorPoolCreateInfo.poolSizeCount = uint32_t(descriptorPoolSize.size());
	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSize.data();
	descriptorPoolCreateInfo.maxSets = maxSets;
	vkCreateDescriptorPool(sVulkanLogicDevice, &descriptorPoolCreateInfo, nullptr, &descriptorPool);
	return descriptorPool;
}
VkDescriptorSet CreateDescriptorSet(const VkDescriptorSetLayout& descriptorSetLayout, const VkDescriptorPool& descriptorPool) {
	VkDescriptorSet descriptorSet;
	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
	descriptorSetAllocateInfo.descriptorPool = descriptorPool;
	vkAllocateDescriptorSets(sVulkanLogicDevice, &descriptorSetAllocateInfo, &descriptorSet);
	return descriptorSet;
}
void UpdateDescriptorSets(const std::vector<VkWriteDescriptorSet>& writeDescriptorSet) {
	vkUpdateDescriptorSets(sVulkanLogicDevice, uint32_t(writeDescriptorSet.size()), writeDescriptorSet.data(), 0, nullptr);
}
VkShaderModule CreateShaderModule(const VkShaderModuleCreateInfo& shaderModuleCreateInfo) {
	VkShaderModule shader;
	vkCreateShaderModule(sVulkanLogicDevice, &shaderModuleCreateInfo, nullptr, &shader);
	return shader;
}
void BeginRendering(VkFramebuffer frame) {
	if (frame==0){
		BeginRenderPass();
	}
}
void UpdateViewport(uint32_t width, uint32_t height) {
	VkViewport viewport = { 0.0f,0.0f,float(width),float(height),0.0f,1.0f };
	VkRect2D scissor = { {0,0},{width,height} };
	vkCmdSetViewport(sMainCommandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(sMainCommandBuffer, 0, 1, &scissor);
}
void UpdateDepthBias(float slopeFactor, float constantFactor) {
	vkCmdSetDepthBias(sMainCommandBuffer, constantFactor, 0.0f, slopeFactor);
}
void BindGraphicPipeline(VkPipeline pipeline) {
	vkCmdBindPipeline(sMainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}
void BindVertexBuffer(VkBuffer buffer) {
	VkBuffer vertexbuffers[] = { buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(sMainCommandBuffer, 0, 1, vertexbuffers, offsets);
}
void BindIndexBuffer(VkBuffer buffer, VkIndexType dateType) {
	vkCmdBindIndexBuffer(sMainCommandBuffer, buffer, 0, dateType);
}
void BindUniformInput(VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet) {
	vkCmdBindDescriptorSets(sMainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,&descriptorSet, 0, nullptr);
}