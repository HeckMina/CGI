#pragma once
#include "AliceRenderPrefix.h"
#include "VertexBuffer.h"
namespace Alice {
	enum class BasicDataType {
		kBasicDataTypeFloat,
		kBasicDataTypeCount
	};
	enum class PrimitiveType {
		kPrimitiveTypeTriangles,
		kPrimitiveTypeTriangleStrip,
		kPrimitiveTypeTriangleLines,
		kPrimitiveTypeCount
	};
	struct AttributeDefine {
		int mInputStreamBufferIndex;
		int mLocation;
		int mComponentCount;
		BasicDataType mDataType;
		bool mbAutoNormalization;
		int mStride;
		int mStartOffset;
		AttributeDefine() :
			mInputStreamBufferIndex(0),mLocation(0),mComponentCount(0),mDataType(BasicDataType::kBasicDataTypeFloat),
			mbAutoNormalization(false),mStride(0),mStartOffset(0) {
		}
	};
	struct AttributeBindingDescription {
		int mBindingPoint;
		int mStride;
		AttributeBindingDescription():mBindingPoint(-1),mStride(-1){}
	};
	class InputAttributeCollection {
	public:
		InputAttributeCollection():mInputAttributeCount(0),mInputVertexBufferCount(0),mPrimitiveType(PrimitiveType::kPrimitiveTypeTriangles) {}
		int mInputAttributeCount;
		int mInputVertexBufferCount;
		PrimitiveType mPrimitiveType;
		AttributeDefine mAttributeDefines[8];
		VertexBuffer* mVertexBuffers[8];
		std::unordered_map<int, AttributeBindingDescription> mAttributeBindingDescription;
	public:
		void SetVertexBuffer(int index, VertexBuffer* vertexBuffer);
		void AppendAttributeInput(int input_stream_buffer_index, int location, int component_count, BasicDataType data_type, bool auto_normalize, int stride, int offset);
	public:
		virtual void Finish() {}
		virtual void Active() {}
		virtual void Deactive() {}
		virtual void Draw() {}
		virtual void SetPrimitiveType(PrimitiveType type) { mPrimitiveType = type; }
	};
}