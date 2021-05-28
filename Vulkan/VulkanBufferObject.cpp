#include "VulkanBufferObject.h"
namespace Alice {
	void VulkanVertexBufferObject::InitBuffer() {
		int bufferSize = GetBufferSize();
		ALICE_GEN_VERTEX_BUFFER(mBuffer, mMemory, bufferSize, mUsage);
	}
	void VulkanVertexBufferObject::SyncDataFromCPUToGPU() {
		if (!mbNeedSyncToGPU) {
			return;
		}
		if (mBuffer == 0) {
			InitBuffer();
		}
		ALICE_BUFFER_SUB_VERTEX_BUFFER(mBuffer, mDataBuffer, GetBufferSize());
		mbNeedSyncToGPU = false;
	}
	void VulkanIndexBufferObject::InitBuffer() {
		int bufferSize = GetBufferSize();
		ALICE_GEN_INDEX_BUFFER(mBuffer, mMemory, bufferSize, mUsage);
	}
	void VulkanIndexBufferObject::SyncDataFromCPUToGPU() {
		if (!mbNeedSyncToGPU) {
			return;
		}
		if (mBuffer == 0) {
			InitBuffer();
		}
		ALICE_BUFFER_SUB_INDEX_BUFFER(mBuffer, mDataBuffer, GetBufferSize());
		mbNeedSyncToGPU = false;
	}
	void VulkanIndexBufferObject::Bind() {
		ALICE_BIND_INDEX_BUFFER(mBuffer, VK_INDEX_TYPE_UINT32);
	}
	void VulkanUniformBufferObject::InitBuffer() {
		int bufferSize = GetBufferSize();
		ALICE_GEN_UNIFORM_BUFFER(mBuffer, mMemory, bufferSize, mUsage);
	}
	void VulkanUniformBufferObject::SyncDataFromCPUToGPU() {
		if (!mbNeedSyncToGPU) {
			return;
		}
		if (mBuffer == 0) {
			InitBuffer();
		}
		ALICE_BUFFER_SUB_UNIFORM_BUFFER(mMemory, mDataBuffer, GetBufferSize());
		mbNeedSyncToGPU = false;
	}
	/*void BufferObject::InitBuffer() {
		int bufferSize = GetBufferSize();
		switch (mBufferObjectType){
		case Alice::BufferObjectType::kBufferObjectTypeVertexBuffer:
			ALICE_GEN_VERTEX_BUFFER(mBuffer, mMemory, bufferSize, mUsage);
			break;
		case Alice::BufferObjectType::kBufferObjectTypeIndexBuffer:
			ALICE_GEN_INDEX_BUFFER(mBuffer, mMemory, bufferSize, mUsage);
			break;
		case Alice::BufferObjectType::kBufferObjectTypeUniformBuffer:
			ALICE_GEN_UNIFORM_BUFFER(mBuffer, mMemory, bufferSize, mUsage);
			break;
		}
	}
	void BufferObject::SetDataBlock(int internal_offset, void* data, int size) {
		float* dataPtr = (float*)((char*)mDataBuffer + internal_offset);
		memcpy(dataPtr, data, size);
		mbNeedSyncToGPU = true;
	}
	void BufferObject::SyncDataFromCPUToGPU() {
		if (!mbNeedSyncToGPU){
			return;
		}
		if (mBuffer==0){
			InitBuffer();
		}
		switch (mBufferObjectType) {
		case Alice::BufferObjectType::kBufferObjectTypeVertexBuffer:
			ALICE_BUFFER_SUB_VERTEX_BUFFER(mBuffer, mDataBuffer, GetBufferSize());
			break;
		case Alice::BufferObjectType::kBufferObjectTypeIndexBuffer:
			ALICE_BUFFER_SUB_INDEX_BUFFER(mBuffer, mDataBuffer, GetBufferSize());
			break;
		case Alice::BufferObjectType::kBufferObjectTypeUniformBuffer:
			ALICE_BUFFER_SUB_UNIFORM_BUFFER(mMemory, mDataBuffer, GetBufferSize());
			break;
		}
		mbNeedSyncToGPU = false;
	}
	void VertexBufferObject::SetFloat4Data(int element_index, int internal_offset, float* data) {
		float* dataPtr = (float*)((char*)mDataBuffer+mStride*element_index+internal_offset);
		dataPtr[0] = data[0];
		dataPtr[1] = data[1];
		dataPtr[2] = data[2];
		dataPtr[3] = data[3];
		mbNeedSyncToGPU = true;
	}
	void VertexBufferObject::SetFloat4Data(int element_index, int internal_offset, float x, float y, float z, float w) {
		float* dataPtr = (float*)((char*)mDataBuffer + mStride * element_index + internal_offset);
		dataPtr[0] = x;
		dataPtr[1] = y;
		dataPtr[2] = z;
		dataPtr[3] = w;
		mbNeedSyncToGPU = true;
	}
	void IndexBufferObject::SetUInt16(int element_index, unsigned short index) {
		unsigned short * dataPtr = (unsigned short*)((char*)mDataBuffer + mStride * element_index);
		*dataPtr = index;
		mbNeedSyncToGPU = true;
	}
	void IndexBufferObject::SetUInt32(int element_index, unsigned int index) {
		unsigned int* dataPtr = (unsigned int*)((char*)mDataBuffer + mStride * element_index);
		*dataPtr = index;
		mbNeedSyncToGPU = true;
	}
	void IndexBufferObject::SetUInt16Data(int start_element_index, unsigned short* indices, int element_count) {
		unsigned short* dataPtr = (unsigned short*)((char*)mDataBuffer + mStride * start_element_index);
		memcpy(dataPtr, indices, sizeof(unsigned short) * element_count);
		mbNeedSyncToGPU = true;
	}
	void IndexBufferObject::SetUInt32Data(int start_element_index, unsigned int* indices, int element_count) {
		unsigned int* dataPtr = (unsigned int*)((char*)mDataBuffer + mStride * start_element_index);
		memcpy(dataPtr, indices, sizeof(unsigned int) * element_count);
		mbNeedSyncToGPU = true;
	}
	void IndexBufferObject::Bind() {

	}*/
}