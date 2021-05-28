#include "VertexBuffer.h"
namespace Alice {
	VertexBuffer::VertexBuffer(int elementSize, int elementCount,BufferUsage usage) :
		mDataBuffer(nullptr),mbNeedSyncToGPU(false),mElementSize(0),mElementCount(0),mBufferSize(0),mBufferUsage(usage) {
		if (mElementSize>0){
			Init(elementSize, elementCount);
		}
	}
	VertexBuffer::~VertexBuffer() {
		VertexBuffer::Reset();
	}
	void VertexBuffer::Init(int elementSize, int elementCount) {
		if (elementSize<=0||elementCount<=0){
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
	void VertexBuffer::Reset() {
		if (mDataBuffer != nullptr) {
			delete[]mDataBuffer;
			mDataBuffer = nullptr;
		}
	}
	void VertexBuffer::SetData(void* data, int size, int offset) {
		mbNeedSyncToGPU = true;
		memcpy(GetBuffer<char>()+offset,data,size);
	}
}