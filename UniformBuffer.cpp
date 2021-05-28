#include "UniformBuffer.h"
namespace Alice {
	UniformBuffer::UniformBuffer(int bufferSize, BufferUsage usage):
		mBufferSize(0),mBufferUsage(usage),mDataBuffer(nullptr),mbNeedSyncToGPU(false){
		if (bufferSize>0){
			Init(bufferSize);
		}
	}
	UniformBuffer::~UniformBuffer() {
		UniformBuffer::Reset();
	}
	void UniformBuffer::Init(int size) {
		if (size != mBufferSize) {
			Reset();
		}
		mBufferSize = size;
		mDataBuffer = new unsigned char[size];
	}
	void UniformBuffer::Reset() {
		if (mDataBuffer!=nullptr){
			delete[]mDataBuffer;
			mDataBuffer = nullptr;
		}
	}
	void UniformBuffer::SetData(void* data, int size, int offset) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset);
		memcpy(dataPtr, data, size);
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat(int offset_in_bytes, const float value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = value;
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat2(int offset_in_bytes, const float x, const float y) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = x;
		*(dataPtr + 1) = y;
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat2(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 2);
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat3(int offset_in_bytes, const float x, const float y, const float z) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = x;
		*(dataPtr + 1) = y;
		*(dataPtr + 2) = z;
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat3(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 3);
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat4(int offset_in_bytes, const float x, const float y, const float z, const float w) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = x;
		*(dataPtr + 1) = y;
		*(dataPtr + 2) = z;
		*(dataPtr + 3) = w;
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetFloat4(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 4);
		mbNeedSyncToGPU = true;
	}
	void UniformBuffer::SetMat4(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 16);
		mbNeedSyncToGPU = true;
	}
}