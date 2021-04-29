#include "ShaderPipeline.h"
#include "GraphicPipeline.h"
#include "Material.h"
#include "InputUniform.h"
namespace Alice {
	Material::Material() {
		mbEnableBlend = false;
		mbEnableDepthTest = true;
		mbEnableCullFace = true;
		mBlendSrcFunc = kBlendFactorSrcAlpha;
		mBlendDstFunc = kBlendFactorOneMinusSrcAlpha;
		mPrimitiveUniformBuffer = new UniformBufferObject(16384);
		mbDirty = true;
	}
	void Material::SetupPrimitiveUniformBuffer(Camera* camera, const glm::mat4& m) {
		mPrimitiveUniformBuffer->SetMat4(0, glm::value_ptr(camera->mProjectionMatrix));
		mPrimitiveUniformBuffer->SetMat4(64, glm::value_ptr(camera->mViewMatrix));
		mPrimitiveUniformBuffer->SetMat4(128, glm::value_ptr(m));
	}
	Material* Material::Clone() {
		Material* ret = new Material;
		ret->mBlendSrcFunc = mBlendSrcFunc;
		ret->mBlendDstFunc = mBlendDstFunc;
		ret->mbEnableBlend = mbEnableBlend;
		ret->mbEnableDepthTest = mbEnableDepthTest;
		ret->mbEnableCullFace = mbEnableCullFace;
		return ret;
	}
}