#pragma once
#include "AliceRenderPrefix.h"
namespace Alice {
	class VertexBuffer {
	public:
		BufferUsage mBufferUsage;
		int mElementSize;
		int mElementCount;
		void* mDataBuffer;
		int mBufferSize;
		bool mbNeedSyncToGPU;
		VertexBuffer(int elementSize = 0, int elementCount = 0,BufferUsage usage = BufferUsage::kBufferUsageStatic);
		virtual ~VertexBuffer();
	private:
		VertexBuffer():
			mDataBuffer(nullptr), mbNeedSyncToGPU(false), mElementSize(0), mElementCount(0), mBufferSize(0), mBufferUsage(BufferUsage::kBufferUsageStatic){}
	public:
		void Init(int elementSize, int elementCount);
		void SetData(void* data, int size, int offset);
	public:
		virtual void Reset();
		virtual void Submit() {}
		virtual void Active() {}
		virtual void Deactive() {}
	public:
		template<typename T>
		inline T* GetBuffer() {
			mbNeedSyncToGPU = true;
			return (T*)mDataBuffer;
		}
		template<typename T>
		inline T* Cast() {
			return (T*)this;
		}
	};
}