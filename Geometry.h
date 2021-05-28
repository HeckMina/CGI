#pragma once
#include "InputAttributeCollection.h"
#include "IndexBuffer.h"
namespace Alice {
	class Submesh {
	public:
		IndexBuffer *mIndexBuffer;
		int mValidIndexCount;
		Submesh() :mIndexBuffer(nullptr),mValidIndexCount(0){}
	};
	class Geometry {
	public:
		InputAttributeCollection *mInputAttributeCollection;
		VertexBuffer *mVertexBuffer;
		PrimitiveType mPrimitiveType;
		std::vector<Submesh*> mSubmeshes;
		Geometry();
		virtual void Init() {}
		void SetVertexCount(int vertex_count);
		void SetIndexCount(int index_count);
		void SetPosition(int index, float x, float y, float z, float w = 1.0f);
		void SetTexcoord(int index, float x, float y, float z = 1.0f, float w = 1.0f);
		void SetNormal(int index, float x, float y, float z, float w = 0.0f);
		void SetIndex(int i, unsigned short index);
		void Submit();
		virtual void Update(float delta) {}
		virtual void BeginDraw();
		virtual void Draw();
	};

}