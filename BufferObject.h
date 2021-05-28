#pragma once
#include "ggl.h"
#include "CGI.h"
namespace Alice {
	class BufferObject {
	public:
		BufferObjectType mBufferObjectType;
		BufferUsageHint mUsage;
		void* mDataBuffer;
		int mBufferSize;
		bool mbNeedSyncToGPU;
		BufferObject(BufferObjectType bufferobject_type, BufferUsageHint usage,int size):
			mDataBuffer(nullptr), mBufferSize(size),mBufferObjectType(bufferobject_type),mUsage(usage), mbNeedSyncToGPU(true){

		}
		virtual ~BufferObject() {
			if (mDataBuffer!=nullptr){
				delete[] mDataBuffer;
			}
		}
		int GetBufferSize() { return mBufferSize; }
		void SetDataBlock(int internal_offset, void* data, int size);
		virtual void InitBuffer() {}
		virtual void SyncDataFromCPUToGPU() {}
	};
	class VertexBufferObject :public BufferObject {
	public:
		int mStride;
		VertexBufferObject(BufferUsageHint usage, int size, int stride):
			BufferObject(BufferObjectType::kBufferObjectTypeVertexBuffer,usage,size),mStride(stride){
			mDataBuffer = new unsigned char[size];
		}
		void SetFloat4Data(int element_index, int internal_offset, float* data);
		void SetFloat4Data(int element_index, int internal_offset, float x,float y=0.0f,float z=0.0f,float w=1.0f);
	};
	class IndexBufferObject :public BufferObject {
	public:
		int mStride;
		IndexBufferObject(BufferUsageHint usage, int size, int stride):
			BufferObject(BufferObjectType::kBufferObjectTypeIndexBuffer,usage, size), mStride(stride) {
			mDataBuffer = new unsigned char[size];
		}
		void SetUInt16(int element_index, unsigned short index);
		void SetUInt32(int element_index, unsigned int index);
		void SetUInt16Data(int start_element_index, unsigned short * indices, int element_count);
		void SetUInt32Data(int start_element_index, unsigned int* indices, int element_count);
		virtual void Bind(){}
	};
	class UniformBufferObject : public BufferObject {
	public:
		VkBuffer mBuffer;
		int mBindingPoint;
		int mUniformDescriptorSetIndex;
		UniformBufferObject(int size = 16384) :
			BufferObject(BufferObjectType::kBufferObjectTypeUniformBuffer, BufferUsageHint::kBufferUsageHintDynamic, size),
			mBindingPoint(-1),
			mUniformDescriptorSetIndex(-1) {
			mDataBuffer = new unsigned char[size];
		}
		void SetFloat(int offset_in_bytes, const float value);
		void SetFloat2(int offset_in_bytes, const float* value);
		void SetFloat2(int offset_in_bytes, const float x, const float y);
		void SetFloat3(int offset_in_bytes, const float* value);
		void SetFloat3(int offset_in_bytes, const float x, const float y, const float z);
		void SetFloat4(int offset_in_bytes, const float* value);
		void SetFloat4(int offset_in_bytes, const float x, const float y, const float z, const float w);
		void SetMat4(int offset_in_bytes, const float* value);
	};
}