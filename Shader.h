#pragma once
#if ALICE_OGL_RENDERER
#include "utils.h"
namespace Alice {
	class Shader {
	public:
		GLuint mProgram;
		GLuint mPosition;
		GLuint mTexcoord;
		GLuint mNormal;
		GLuint mTangent;
		GLint mModelMatrixLocation, mViewMatrixLocation, mProjectionMatrixLocation, mITModelMatrixLocation;
		GLint mPositionLocation, mTexcoordLocation, mNormalLocation, mTangentLocation;
		void Init(const char* vs, const char* fs);
		void BeginDraw(Camera* camera, const glm::mat4& model);
		void EndDraw();
	};
}
#endif