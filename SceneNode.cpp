#include "SceneNode.h"
#include "Geometry.h"
#include "Material.h"
#include "InputUniform.h"
#include "shader.h"
#include "utils.h"
#include "Vulkan/VulkanBufferObject.h"
namespace Alice {
	SceneNode::SceneNode() :mGraphicPipeline(nullptr),mUniformInputLayoutInstance(nullptr) {

	}
	void SceneNode::Update(UniformBufferObject* viewUniformBuffer,float delta) {
		if (mUniformInputLayoutInstance == nullptr) {
			UniformInputLayoutDescriptor::Register(&mUniformInputDescription);
			mUniformInputLayoutInstance = UniformInputLayoutDescriptor::CreateUniformInputLayoutInstance(mUniformInputDescription.GetName());
		}
		glm::mat4 itModel = glm::inverseTranspose(mModelMatrix);
		bool needUpdateUniformInput = false;
		mMaterial->mPrimitiveUniformBuffer->SetMat4(0, glm::value_ptr(mModelMatrix));
		mMaterial->mPrimitiveUniformBuffer->SetMat4(sizeof(float) * 16, glm::value_ptr(itModel));
		mMaterial->mPrimitiveUniformBuffer->SyncDataFromCPUToGPU();
		if (!mUniformInputLayoutInstance->IsUniformBufferBinded(0, viewUniformBuffer)) {
			mUniformInputLayoutInstance->BindingUniformBuffer(0, viewUniformBuffer);
			needUpdateUniformInput = true;
		}
		if (!mUniformInputLayoutInstance->IsUniformBufferBinded(1, mMaterial->mPrimitiveUniformBuffer)) {
			mUniformInputLayoutInstance->BindingUniformBuffer(1, mMaterial->mPrimitiveUniformBuffer);
			needUpdateUniformInput = true;
		}
		if (needUpdateUniformInput) {
			mUniformInputLayoutInstance->Update();
		}
		if (mGraphicPipeline == nullptr) {
			mGraphicPipeline = new GraphicPipeline;
			mGraphicPipeline->mGraphicPipelineSetting.mColorBlendSetting.AppendColorBlendSetting(true, 0, kBlendOperationAdd, kBlendOperationAdd, kBlendFactorSrcAlpha, kBlendFactorOneMinusSrcAlpha, kBlendFactorOneMinusSrcAlpha, kBlendFactorOneMinusSrcAlpha);
			mGraphicPipeline->Compile(mGeometry->mInputAttributeCollection,&mMaterial->mShaderPipeline,mUniformInputLayoutInstance->mUniformInputLayoutDescriptor->mDescriptorSetLayout);
		}
		//update self
		mGeometry->Update(delta);
		if (mNext != nullptr) {
			Next<SceneNode>()->Update(viewUniformBuffer,delta);
		}
	}
	void SceneNode::Render() {
		ALICE_BIND_GRAPHIC_PIPELINE(mGraphicPipeline->mGraphicPipeline);
		mGeometry->mInputAttributeCollection->Active();
		mGeometry->mSubmeshes[0]->mIndexBuffer->Active();
		ALICE_BIND_UNIFORM_INPUT(mGraphicPipeline->mPipelineLayout, mUniformInputLayoutInstance->mDescriptorSet);
		ALICE_DRAW_PRIMITIVES_INDEXED(mGeometry->mSubmeshes[0]->mValidIndexCount);
		if (mNext != nullptr) {
			Next<SceneNode>()->Render();
		}
	}
}