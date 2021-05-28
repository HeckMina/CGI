#include "Geometry.h"
#include "utils.h"
#include "Vulkan/VulkanInputAttributeCollection.h"
#include "Vulkan/VulkanBufferObject.h"
namespace Alice {
	Geometry::Geometry() {
		mbDrawWidthIBO = false;
		mPrimitiveType = kPrimitiveTypeTriangles;
		mVBO = nullptr;
		mIBO = nullptr;
	}
	void Geometry::SetIndexCount(int index_count) {
		mbDrawWidthIBO = true;
		mIndexCount = index_count;
		mIBO = new VulkanIndexBufferObject(BufferUsageHint::kBufferUsageHintStatic, sizeof(unsigned int) * index_count,sizeof(unsigned int));
	}
	void Geometry::SetVertexCount(int vertex_count) {
		mVertexCount = vertex_count;
		mVBO = new VulkanVertexBufferObject(BufferUsageHint::kBufferUsageHintStatic, sizeof(Vertex) * vertex_count, sizeof(Vertex));
		mInputAttributeCollection = new VulkanInputAttributeCollection;
		mInputAttributeCollection->SetPolygonMode(kPrimitiveTypeTriangles);
		mInputAttributeCollection->AppendAttributeInput(0, 0, 4, kBasicDataTypeFloat, false, sizeof(Vertex), 0);
		mInputAttributeCollection->AppendAttributeInput(0, 1, 4, kBasicDataTypeFloat, false, sizeof(Vertex), sizeof(float) * 4);
		mInputAttributeCollection->AppendAttributeInput(0, 2, 4, kBasicDataTypeFloat, false, sizeof(Vertex), sizeof(float) * 8);
		mInputAttributeCollection->AppendAttributeInput(0, 3, 4, kBasicDataTypeFloat, false, sizeof(Vertex), sizeof(float) * 12);
		mInputAttributeCollection->SetVertexBuffer(0, mVBO);
	}
	void Geometry::SetIndex(int i, unsigned short index) {
		mIBO->SetUInt32(i, index);
	}
	void Geometry::SetPosition(int index, float x, float y, float z, float w) {
		mVBO->SetFloat4Data(index, 0, x,y,z,w);
	}
	void Geometry::SetTexcoord(int index, float x, float y, float z, float w) {
		mVBO->SetFloat4Data(index, sizeof(float)*4, x, y, z, w);
	}
	void Geometry::SetNormal(int index, float x, float y, float z, float w) {
		mVBO->SetFloat4Data(index, sizeof(float) * 8, x, y, z, w);
	}
	void Geometry::Submit() {
		mVBO->SyncDataFromCPUToGPU();
		if (mbDrawWidthIBO) {
			mIBO->SyncDataFromCPUToGPU();
		}
	}
	void Geometry::BeginDraw() {
		//ALICE_BIND_VERTEX_BUFFER(mVBO->mBuffer);
		if (mbDrawWidthIBO) {
		}
	}
	void Geometry::Draw() {
		if (mbDrawWidthIBO) {
			ALICE_DRAW_PRIMITIVES_INDEXED(mPrimitiveType,mIndexCount);
		}
		else {
			ALICE_DRAW_PRIMITIVES(mPrimitiveType, mVertexCount);
		}
	}
}