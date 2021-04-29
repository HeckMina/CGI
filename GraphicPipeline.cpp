#include "GraphicPipeline.h"
#include "VulkanUtils.h"

namespace Alice {
	std::vector<VkVertexInputAttributeDescription> GenVertexInputAttributeDescription(const InputAttributeDescription* inputAttributesDescription) {
		std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
		attribute_descriptions.resize(inputAttributesDescription->mCurrentInputAttributeCount);
		for (int i = 0; i < inputAttributesDescription->mCurrentInputAttributeCount; ++i) {
			attribute_descriptions[i].binding = inputAttributesDescription->mAttributeDefines[i].mInputStreamBufferIndex;
			attribute_descriptions[i].location = inputAttributesDescription->mAttributeDefines[i].mLocation;
			VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
			if (inputAttributesDescription->mAttributeDefines[i].mDataType == kBasicDataTypeFloat) {
				if (inputAttributesDescription->mAttributeDefines[i].mComponentCount == 4) {
					format = VK_FORMAT_R32G32B32A32_SFLOAT;
				}
			}
			attribute_descriptions[i].format = format;
			attribute_descriptions[i].offset = inputAttributesDescription->mAttributeDefines[i].mStartOffset;
		}
		return attribute_descriptions;
	}
	std::vector<VkVertexInputBindingDescription> GenVertexInputBindingDescription(const InputAttributeDescription* inputAttributesDescription) {
		int attributeBindingDescriptionCount = inputAttributesDescription->mAttributeBindingDescription.size();
		std::vector<VkVertexInputBindingDescription> attribute_binding_descriptions;
		attribute_binding_descriptions.resize(attributeBindingDescriptionCount);
		int bindingDescriptionIndex = 0;
		for (auto iter = inputAttributesDescription->mAttributeBindingDescription.begin(); iter != inputAttributesDescription->mAttributeBindingDescription.end(); ++iter) {
			attribute_binding_descriptions[bindingDescriptionIndex].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			attribute_binding_descriptions[bindingDescriptionIndex].binding = iter->second->mBindingPoint;
			attribute_binding_descriptions[bindingDescriptionIndex++].stride = iter->second->mStride;
		}
		return attribute_binding_descriptions;
	}
	VkPipelineVertexInputStateCreateInfo GenPipelineVertexInputStateCreateInfo(
		const std::vector<VkVertexInputBindingDescription>&vertexInputBindingDescriptions,
		const std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescriptions) {
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
		vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_state_create_info.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();
		vertex_input_state_create_info.vertexBindingDescriptionCount = vertexInputBindingDescriptions.size();
		vertex_input_state_create_info.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
		vertex_input_state_create_info.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
		return vertex_input_state_create_info;
	}
	std::vector<VkPipelineShaderStageCreateInfo> GenPipelineShaderStageCreateInfo(const ShaderPipeline* shaderPipeline) {
		std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
		for (int i = 0; i < 5; ++i) {
			if (shaderPipeline->mShaderStages[i] != nullptr) {
				VkShaderStageFlagBits shader_stage_flag_bit = VK_SHADER_STAGE_VERTEX_BIT;
				switch (shaderPipeline->mShaderStages[i]->GetType()) {
				case kShaderTypeVertex:
					break;
				case kShaderTypeFragment:
					shader_stage_flag_bit = VK_SHADER_STAGE_FRAGMENT_BIT;
					break;
				}
				VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					nullptr,0,shader_stage_flag_bit,shaderPipeline->mShaderStages[i]->GetCGIShader(),"main",nullptr
				};
				pipelineShaderStageCreateInfos.push_back(pipelineShaderStageCreateInfo);
			}
		}
		return pipelineShaderStageCreateInfos;
	}
	ALICE_GRAPHIC_PIPELINE GenGraphicPipeline(const VkDescriptorSetLayout& descriptorSetLayout) {
		VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.pSetLayouts = &descriptorSetLayout;
		pipeline_layout_create_info.setLayoutCount = 1;
		pipeline_layout_create_info.pPushConstantRanges = nullptr;
		pipeline_layout_create_info.pushConstantRangeCount = 0;
		return ALICE_CREATE_GRAPHIC_PIPELINE_LAYOUT(pipeline_layout_create_info);
	}
	void GraphicPipeline::Compile(
		const InputAttributeDescription* inputAttributesDescription, 
		const ShaderPipeline* shaderPipeline, 
		const VkDescriptorSetLayout& descriptorSetLayout) {
#if ALICE_VULKAN_RENDERER
		mVertexInputBindingDescriptions = GenVertexInputBindingDescription(inputAttributesDescription);
		mVertexInputAttributeDescriptions = GenVertexInputAttributeDescription(inputAttributesDescription);
		VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = GenPipelineVertexInputStateCreateInfo(mVertexInputBindingDescriptions, mVertexInputAttributeDescriptions);
		mPipelineShaderStageDescriptions = GenPipelineShaderStageCreateInfo(shaderPipeline);
		mPipelineLayout = GenGraphicPipeline(descriptorSetLayout);

		VkDynamicState dynamic_state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_BIAS };
		VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
		dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state_create_info.dynamicStateCount = RAW_C_ARRAY_ELEMENT_COUNT(dynamic_state);
		dynamic_state_create_info.pDynamicStates = dynamic_state;
		
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {0.0f,0.0f,0.0f,0.0f,0.0f,1.0f};
		VkRect2D scissor = {{0,0},{0,0}};
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,nullptr,0,1,&viewport,1,&scissor};

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.lineWidth = 1.0f;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_TRUE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.minDepthBounds = 0.0f;
		depthStencilStateCreateInfo.maxDepthBounds = 1.0f;
		depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.front = {};
		depthStencilStateCreateInfo.back = {};

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.minSampleShading = 1.0f;
		multisampleStateCreateInfo.pSampleMask = nullptr;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		int color_attachment_count = 0;
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState[8] = {};
		for (int i = 0; i < mGraphicPipelineSetting.mColorBlendSetting.mColorAttachmentCount; ++i) {
			colorBlendAttachmentState[color_attachment_count].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachmentState[color_attachment_count].blendEnable = mGraphicPipelineSetting.mColorBlendSetting.mBlendSetting[i].mbEnableBlend;
			colorBlendAttachmentState[color_attachment_count].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachmentState[color_attachment_count].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachmentState[color_attachment_count].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachmentState[color_attachment_count].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachmentState[color_attachment_count].colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachmentState[color_attachment_count++].alphaBlendOp = VK_BLEND_OP_ADD;
		}
		colorBlendStateCreateInfo.attachmentCount = color_attachment_count;
		colorBlendStateCreateInfo.pAttachments = colorBlendAttachmentState;

		VkGraphicsPipelineCreateInfo ci = {};
		ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		ci.stageCount = mPipelineShaderStageDescriptions.size();
		ci.pStages = mPipelineShaderStageDescriptions.data();
		ci.pVertexInputState = &vertex_input_state_create_info;
		ci.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		ci.pViewportState = &viewportStateCreateInfo;
		ci.pRasterizationState = &rasterizationStateCreateInfo;
		ci.pDepthStencilState = &depthStencilStateCreateInfo;
		ci.pColorBlendState = &colorBlendStateCreateInfo;
		ci.pDynamicState = &dynamic_state_create_info;
		ci.pMultisampleState = &multisampleStateCreateInfo;
		ci.layout = mPipelineLayout;
		//ci.renderPass = mGraphicPipelineSetting.mRenderPass->mRenderPass;
		ci.subpass = 0;
		ci.basePipelineHandle = VK_NULL_HANDLE;
		ci.basePipelineIndex = -1;
		mGraphicPipeline = ALICE_CREATE_GRAPHIC_PIPELINE(ci);
#endif
	}
}