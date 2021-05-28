#include "IndexBuffer.h"
namespace Alice {
	IndexBuffer::IndexBuffer(int elementSize, int elementCount, BufferUsage usage) :
		mDataBuffer(nullptr), mbNeedSyncToGPU(false), mElementSize(0), mElementCount(0), mBufferSize(0), mBufferUsage(usage) {
		if (mElementSize > 0) {
			Init(elementSize, elementCount);
		}
	}
	IndexBuffer::~IndexBuffer() {
		IndexBuffer::Reset();
	}
	void IndexBuffer::Init(int elementSize, int elementCount) {
		if (elementSize <= 0 || elementCount <= 0) {
			return;
		}
		int bufferSizeNeeded = elementSize * elementCount;
		if (bufferSizeNeeded != mBufferSize) {
			Reset();
		}
		mBufferSize = bufferSizeNeeded;
		mDataBuffer = new unsigned char[bufferSizeNeeded];
		mElementSize = elementSize;
		mElementCount = elementCount;
	}
	void IndexBuffer::Reset() {
		if (mDataBuffer != nullptr) {
			delete[]mDataBuffer;
			mDataBuffer = nullptr;
		}
	}
	void IndexBuffer::SetData(void* data, int size, int offset) {
		mbNeedSyncToGPU = true;
		memcpy(GetBuffer<char>() + offset, data, size);
	}
}