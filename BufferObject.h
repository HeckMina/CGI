#pragma once
#include "ggl.h"
#include "CGI.h"
namespace Alice {
	enum class BufferObjectType {
		kBufferObjectTypeVertexBuffer,
		kBufferObjectTypeIndexBuffer,
		kBufferObjectTypeUniformBuffer,
		kBufferObjectTypeCount
	};
	enum class BufferUsageHint{
		kBufferUsageHintStatic,
		kBufferUsageHintDynamic,
		kBufferUsageHintCount
	};
	class BufferObject {
	public:
		ALICE_GPU_BUFFER mBuffer;
		ALICE_GPU_MEMORY mMemory;
		BufferObjectType mBufferObjectType;
		BufferUsageHint mUsage;
		void* mDataBuffer;
		int mBufferSize;
		bool mbNeedSyncToGPU;
		BufferObject(BufferObjectType bufferobject_type, BufferUsageHint usage,int size):
			mBuffer(0),mMemory(0), mDataBuffer(nullptr), mBufferSize(size),mBufferObjectType(bufferobject_type),mUsage(usage), mbNeedSyncToGPU(true){

		}
		virtual ~BufferObject() {
			if (mBuffer != 0) {
				ALICE_DELETE_GPU_BUFFER(mBuffer);
				ALICE_FREE_GPU_MEMORY(mMemory);
			}
			if (mDataBuffer!=nullptr){
				delete[] mDataBuffer;
			}
		}
		virtual int GetBufferSize() { return mBufferSize; }
		void InitBuffer();
		void SetDataBlock(int internal_offset, void* data, int size);
		void SyncDataFromCPUToGPU();
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
		void Bind();
	};
}