#pragma once
#include "InputAttributeCollection.h"
#include "InputUniform.h"
#include "ShaderPipeline.h"
#include "ViewportSetting.h"
#include "RasterizerSetting.h"
#include "DepthStencilSetting.h"
#include "MultiSampleSetting.h"
#include "RenderTargetSetting.h"
#include "Material.h"
namespace Alice {
	class GraphicPipelineSetting {
	public:
		RasterizerSetting mRasterizerSetting;
		DepthStencilSetting mDepthStencilSetting;
		MultiSampleSetting mMultiSampleSetting;
		ColorBlendSetting mColorBlendSetting;
		CGRenderPass *mRenderPass;
		GraphicPipelineSetting():mRenderPass(nullptr) {}
		~GraphicPipelineSetting() {}
	};
	class GraphicPipeline {
	public:
		std::vector<VkVertexInputBindingDescription> mVertexInputBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> mVertexInputAttributeDescriptions;
		ALICE_GRAPHIC_PIPELINE mGraphicPipeline;
		VkPipelineLayout mPipelineLayout;
		GraphicPipelineSetting mGraphicPipelineSetting;
		void Compile(const InputAttributeCollection*inputAttributesDescription, const ShaderPipeline*shaderPipeline,const VkDescriptorSetLayout &layout);
	};
}