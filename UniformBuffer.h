#pragma once
#include "AliceRenderPrefix.h"
namespace Alice {
	class UniformBuffer {
	public:
		BufferUsage mBufferUsage;
		void* mDataBuffer;
		int mBufferSize;
		bool mbNeedSyncToGPU;
		UniformBuffer(int bufferSize = 16384, BufferUsage usage = BufferUsage::kBufferUsageStatic);
		virtual ~UniformBuffer();
	private:
		UniformBuffer() :
			mDataBuffer(nullptr), mbNeedSyncToGPU(false), mBufferSize(0), mBufferUsage(BufferUsage::kBufferUsageStatic) {}
	public:
		void Init(int bufferSize);
		void SetData(void* data, int size, int offset);
		void SetFloat(int offset_in_bytes, const float value);
		void SetFloat2(int offset_in_bytes, const float* value);
		void SetFloat2(int offset_in_bytes, const float x, const float y);
		void SetFloat3(int offset_in_bytes, const float* value);
		void SetFloat3(int offset_in_bytes, const float x, const float y, const float z);
		void SetFloat4(int offset_in_bytes, const float* value);
		void SetFloat4(int offset_in_bytes, const float x, const float y, const float z, const float w);
		void SetMat4(int offset_in_bytes, const float* value);
	public:
		virtual void Reset();
		virtual void Submit() {}
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