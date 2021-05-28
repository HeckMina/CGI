#pragma once
#include "../BufferObject.h"
namespace Alice {
	class VulkanVertexBufferObject :public VertexBufferObject {
	public:
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		int mStride;
		VulkanVertexBufferObject(BufferUsageHint usage, int size, int stride):
			VertexBufferObject(usage, size, stride), mBuffer(0), mMemory(0), mStride(stride){
			mDataBuffer = new unsigned char[size];
		}
		virtual ~VulkanVertexBufferObject() {
			if (mBuffer != 0) {
				ALICE_DELETE_GPU_BUFFER(mBuffer);
				ALICE_FREE_GPU_MEMORY(mMemory);
			}
		}
		virtual void InitBuffer();
		virtual void SyncDataFromCPUToGPU();
	};
	class VulkanIndexBufferObject :public IndexBufferObject {
	public:
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		int mStride;
		VulkanIndexBufferObject(BufferUsageHint usage, int size, int stride):
			IndexBufferObject(usage, size, stride), mBuffer(0), mMemory(0), mStride(stride) {
			mDataBuffer = new unsigned char[size];
		}
		~VulkanIndexBufferObject() {
			if (mBuffer != 0) {
				ALICE_DELETE_GPU_BUFFER(mBuffer);
				ALICE_FREE_GPU_MEMORY(mMemory);
			}
		}
		virtual void InitBuffer();
		virtual void SyncDataFromCPUToGPU();
		virtual void Bind();
	};
	class VulkanUniformBufferObject : public UniformBufferObject {
	public:
		VkBuffer mBuffer;
		VkDeviceMemory mMemory;
		VulkanUniformBufferObject(int size = 16384) :
			UniformBufferObject(size), mBuffer(0), mMemory(0) {
		}
		~VulkanUniformBufferObject() {
			if (mBuffer != 0) {
				ALICE_DELETE_GPU_BUFFER(mBuffer);
				ALICE_FREE_GPU_MEMORY(mMemory);
			}
		}
		virtual void InitBuffer();
		virtual void SyncDataFromCPUToGPU();
	};
}