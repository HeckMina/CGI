#pragma once
#include "ggl.h"
#include "BufferObject.h"
namespace Alice {
	enum PrimitiveType {
		kPrimitiveTypeTriangles,
		kPrimitiveTypeTriangleStrip,
		kPrimitiveTypeCount
	};
	enum BasicDataType{
		kBasicDataTypeFloat,
		kBasicDataTypeCount
	};
	struct AttributeDefine {
		int mInputStreamBufferIndex;
		int mLocation;
		int mComponentCount;
		BasicDataType mDataType;
		bool mbAutoNormalization;
		int mStride;
		int mStartOffset;
		AttributeDefine() {
			mInputStreamBufferIndex = 0;
		}
	};
	struct AttributeBindingDescription {
		int mBindingPoint;
		int mStride;
		AttributeBindingDescription():mBindingPoint(-1),mStride(-1){}
	};
	class InputAttributeDescription {
	public:
		InputAttributeDescription() {

		}
		AttributeDefine mAttributeDefines[8];
		int mCurrentInputAttributeCount;
		PrimitiveType mPrimitiveType;
		std::unordered_map<int, AttributeBindingDescription> mAttributeBindingDescription;
		void SetPolygonMode(PrimitiveType type) {
			mPrimitiveType = type;
		}
	public:
		template<int input_stream_buffer_index, int location, int component_count, BasicDataType data_type, bool auto_normalize, int stride, int offset>
		void AppendAttributeInput() {
			mAttributeDefines[mCurrentInputAttributeCount].mInputStreamBufferIndex = input_stream_buffer_index;
			mAttributeDefines[mCurrentInputAttributeCount].mLocation = location;
			mAttributeDefines[mCurrentInputAttributeCount].mComponentCount = component_count;
			mAttributeDefines[mCurrentInputAttributeCount].mDataType = data_type;
			mAttributeDefines[mCurrentInputAttributeCount].mbAutoNormalization = auto_normalize;
			mAttributeDefines[mCurrentInputAttributeCount].mStride = stride;
			mAttributeDefines[mCurrentInputAttributeCount++].mStartOffset = offset;
			auto iter = mAttributeBindingDescription.find(input_stream_buffer_index);
			if (iter == mAttributeBindingDescription.end()) {
				AttributeBindingDescription abd;
				abd.mBindingPoint = input_stream_buffer_index;
				abd.mStride = stride;
				mAttributeBindingDescription.insert(std::pair<int, AttributeBindingDescription>(input_stream_buffer_index, abd));
			}
		}
	};
}