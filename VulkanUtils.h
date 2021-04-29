#pragma once
#include "ggl.h"
#if ALICE_VULKAN_RENDERER
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
void InitVulkan(void*hwnd);
void SetMSAASampleCount(int sample_count);
void OnVulkanViewportChanged(uint32_t width,uint32_t height);
uint32_t GetViewportWidth();
uint32_t GetViewportHeight();
class TextureObject {
public:
	VkImage mImage;
	VkDeviceMemory mDeviceMemory;
	VkImageView mImageView;
	TextureObject():mImage(0),mDeviceMemory(0), mImageView(0){}
	virtual ~TextureObject();
};
class Texture : public TextureObject{
public:
	VkSampler mSampler;
};
void GenImage(VkImage&image, uint32_t width, uint32_t height, VkFormat format, VkSampleCountFlagBits sample_count, VkImageUsageFlags usage, int mipmap_level_count=1);
void AllocateImageMemory(VkImage& image,VkDeviceMemory&memory);
void GenImageView2D(VkImageView& imageView, VkImage image, VkFormat format, uint32_t base_mip_level = 0, uint32_t mipmap_level_count = 1,
	uint32_t layer_count = 1, uint32_t base_layer_index = 0, VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT);
VkCommandBuffer BeginRenderPass(VkCommandBuffer commandbuffer = nullptr);
void GenCommandBuffer(VkCommandBuffer* commandbuffer, int count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
void BeginCommandBuffer(VkCommandBuffer* commandbuffer, VkCommandBufferUsageFlagBits usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
void EndRendering(VkCommandBuffer commandBuffer);
void VulkanSwapBuffers();
void SetMainCommandBuffer(VkCommandBuffer commandBuffer);
VkCommandBuffer GetMainCommandBuffer();
void DeleteGPUBuffer(VkBuffer buffer);
void FreeGPUMemory(VkDeviceMemory memory);
VkResult GenBuffer(VkBuffer & buffer, VkDeviceMemory & buffermemory, VkDeviceSize size, VkBufferUsageFlags useFlags, VkMemoryPropertyFlags properties);
void BufferSubDataToDeviceMemory(VkBuffer buffer, VkBufferUsageFlags usage, const void* data, VkDeviceSize size);
void BufferSubDataToHostVisiableMemory(VkDeviceMemory& buffermemory,const void*data,int size);
VkPipeline CreateGraphicPipeline(VkGraphicsPipelineCreateInfo graphicPipelineCreateInfo);
VkPipelineLayout CreateGraphicPipelineLayout(VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo);
VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBinding);
VkDescriptorPool CreateDescriptorPool(const std::vector<VkDescriptorPoolSize>& descriptorPoolSize,int maxSets);
VkDescriptorSet CreateDescriptorSet(const VkDescriptorSetLayout & descriptorSetLayout,const VkDescriptorPool& descriptorPool);
void UpdateDescriptorSets(const std::vector<VkWriteDescriptorSet>& writeDescriptorSet);
VkShaderModule CreateShaderModule(const VkShaderModuleCreateInfo& shaderModuleCreateInfo);
void BeginRendering(VkFramebuffer frame);
void UpdateViewport(uint32_t width, uint32_t height);
void UpdateDepthBias(float slopeFactor, float constantFactor);
void BindGraphicPipeline(VkPipeline pipeline);
void BindVertexBuffer(VkBuffer buffer);
void BindIndexBuffer(VkBuffer buffer, VkIndexType dateType);
void BindUniformInput(VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet);
#define GenVertexBuffer(buffer, buffermemory, size) \
	GenBuffer(buffer, buffermemory, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
#define BufferSubVertexData(buffer,data,size) \
	BufferSubDataToDeviceMemory(buffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, data, size);
#define GenIndexBuffer(buffer, buffermemory, size) \
	GenBuffer(buffer,buffermemory,size,VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
#define BufferSubIndexData(buffer,data,size) \
	BufferSubDataToDeviceMemory(buffer,VK_BUFFER_USAGE_TRANSFER_SRC_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT,data,size);
#define GenUniformBuffer(buffer, buffermemory, size) \
	GenBuffer(buffer, buffermemory,size,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
#define BufferUniformBufferData(buffer,data,size) \
	BufferSubDataToHostVisiableMemory(buffer, data, size)
#endif