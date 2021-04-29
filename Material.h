#pragma once
#include "CGI.h"
namespace Alice {
	class Camera;
	class GraphicPipeline;
	class UniformBufferObject;
	class ShaderPipeline;
	class Material {
	public:
		BlendFactor mBlendSrcFunc, mBlendDstFunc;
		bool mbEnableBlend;
		bool mbEnableDepthTest;
		bool mbEnableCullFace;
		bool mbDirty;
		ShaderPipeline mShaderPipeline;
		UniformBufferObject* mPrimitiveUniformBuffer;
		GraphicPipeline *mGraphicPipeline;
		Material();
		void SetupPrimitiveUniformBuffer(Camera* camera, const glm::mat4& m);
		Material* Clone();
	};
}
