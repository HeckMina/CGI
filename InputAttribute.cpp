#include "InputAttribute.h"
#include "utils.h"
namespace Alice {
#if ALICE_OGL_RENDERER
	InputStream::InputStream(GLenum bufferobject_type, GLenum usage) {
		mBufferObjectType = bufferobject_type;
		mUsageHint = usage;
		mBufferObject = 0;
		mBufferSize = 0;
	}
	InputStream::~InputStream() {

	}
	int InputStream::GetBufferSize() {
		return mBufferSize;
	}
	void InputStream::SubmitData(void* data, int offset, size_t size) {
		if (mBufferSize < size) {
			if (mBufferObject != 0) {
				glDeleteBuffers(1, &mBufferObject);
				mBufferObject = 0;
			}
		}
		if (mBufferObject == 0) {
			mBufferObject = CreateBufferObject(mBufferObjectType, size, mUsageHint, data);
		}
		else {
			UpdateBufferObject(mBufferObject, mBufferObjectType, data, size, offset);
		}
		mBufferSize = size;
	}
	InputAttribute::InputAttribute() {
		mCurrentInputAttributeCount = 0;
		mObject = 0;
	}
	void InputAttribute::SetInputStream(int index, BufferObject* input_stream) {
		mInputStreams[index] = input_stream;
	}
	void InputAttribute::SetPrimitiveType(PrimitiveType mode) {
		mPrimitiveType = mode;
	}
	void InputAttribute::Finish() {
		if (mObject != 0) {
			OGL_CALL(glDeleteVertexArrays(1, &mObject));
			mObject = 0;
		}
		OGL_CALL(glGenVertexArrays(1, &mObject));
		OGL_CALL(glBindVertexArray(mObject));
		for (int i = 0; i < mCurrentInputAttributeCount; ++i) {
			const AttributeDefine& attribute = mAttributeDefines[i];
			OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, mInputStreams[(int)attribute.mInputStreamBufferIndex]->mBufferObject));
			OGL_CALL(glEnableVertexAttribArray(attribute.mLocation));
			OGL_CALL(glVertexAttribPointer(attribute.mLocation, attribute.mComponentCount, attribute.mDataType, attribute.mbAutoNormalization, attribute.mStride, (void*)attribute.mStartOffset));
		}
		//OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
		OGL_CALL(glBindVertexArray(0));
	}
#endif
}