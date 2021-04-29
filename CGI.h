#pragma once
#include "utils.h"
enum ColorBufferMask {
	kColorBufferMaskR,
	kColorBufferMaskG,
	kColorBufferMaskB,
	kColorBufferMaskA,
	kColorBufferMaskCount
};
enum BlendOperation {
	kBlendOperationAdd = 0,
	kBlendOperationSubstract = 1,
	kBlendOperationReverseSubstract = 2,
	kBlendOperationMin = 3,
	kBlendOperationMax = 4
};
enum BlendFactor {
	kBlendFactorZero,
	kBlendFactorOne,
	kBlendFactorSrcColor,
	kBlendFactorOneMinusSrcColor,
	kBlendFactorDstColor,
	kBlendFactorOneMinusDstColor,
	kBlendFactorSrcAlpha,
	kBlendFactorOneMinusSrcAlpha,
	kBlendFactorDstAlpha,
	kBlendFactorOneMinusDstAlpha,
	kBlendFactorConstantColor,
	kBlendFactorOneMinusConstantColor,
	kBlendFactorConstantAlpha,
	kBlendFactorOneMinusConstantAlpha,
	kBlendFactorSrcAlphaSaturate,
	kBlendFactorSrc1Color,
	kBlendFactorOneMinusSrc1Color,
	kBlendFactorSrc1Alpha,
	kBlendFactorOneMinusSrc1Alpha
	};
enum LogicOperation {
	kLogicOperationClear,
	kLogicOperationAnd,
	kLogicOperationAndReverse,
	kLogicOperationCopy,
	kLogicOperationAndInverted,
	kLogicOperationNoOperation,
	kLogicOperationXor,
	kLogicOperationOr,
	kLogicOperationNor,
	kLogicOperationEquivalent,
	kLogicOperationInvert,
	kLogicOperationOrReverse,
	kLogicOperationCopyInverted,
	kLogicOperationOrInverted,
	kLogicOperationNand,
	kLogicOperationSet
};
#ifdef ALICE_OGL_RENDERER
#include "ggl.h"
#define ALICE_GPU_BUFFER GLuint
#define ALICE_INPUT_ATTRIBUTE_COLLECTION GLuint
#define ALICE_GPU_MEMORY void*
#define ALICE_DELETE_GPU_BUFFER(buffer) glDeleteBuffers(1,&buffer)
#define ALICE_FREE_GPU_MEMORY(memory)
#define ALICE_GEN_VERTEX_BUFFER(buffer, buffermemory, size, usage_hint) GenVertexBuffer(buffer, size, GL_STATIC_DRAW)
#define ALICE_GEN_INDEX_BUFFER(buffer, buffermemory, size, usage_hint) GenIndexBuffer(buffer, size, GL_STATIC_DRAW)
#define ALICE_GEN_UNIFORM_BUFFER(buffer, buffermemory, size, usage_hint) GenUniformBuffer(buffer, buffermemory, size)
#define ALICE_BUFFER_SUB_VERTEX_BUFFER(buffer, data, size) BufferSubVertexData(buffer, data, size)
#define ALICE_BUFFER_SUB_INDEX_BUFFER(buffer, data, size) BufferSubIndexData(buffer, data, size)
#define ALICE_BUFFER_SUB_UNIFORM_BUFFER(buffer, data, size) BufferUniformBufferData(buffer, data, size)
#define ALICE_BIND_VERTEX_BUFFER(buffer) glBindBuffer(GL_ARRAY_BUFFER,buffer)
#define ALICE_DRAW_PRIMITIVES(primitive,vertex_count) glDrawArrays(primitive, 0, vertex_count)
#define ALICE_DRAW_PRIMITIVES_INDEXED(primitive,index_count) glDrawElements(primitive, index_count, GL_UNSIGNED_INT, 0)
#elif ALICE_VULKAN_RENDERER
#include "VulkanUtils.h"
#define ALICE_GPU_BUFFER VkBuffer
#define ALICE_GPU_MEMORY VkDeviceMemory
#define ALICE_INPUT_ATTRIBUTE_COLLECTION int
#define ALICE_GRAPHIC_PIPELINE VkPipeline
#define ALICE_FRAMEBUFFER VkFramebuffer
#define ALICE_DELETE_GPU_BUFFER(buffer) DeleteGPUBuffer(buffer)
#define ALICE_FREE_GPU_MEMORY(memory) FreeGPUMemory(memory)
#define ALICE_GEN_VERTEX_BUFFER(buffer, buffermemory, size, usage_hint) GenVertexBuffer(buffer, buffermemory, size)
#define ALICE_GEN_INDEX_BUFFER(buffer, buffermemory, size, usage_hint) GenIndexBuffer(buffer, buffermemory, size)
#define ALICE_GEN_UNIFORM_BUFFER(buffer, buffermemory, size, usage_hint) GenUniformBuffer(buffer, buffermemory, size)
#define ALICE_BUFFER_SUB_VERTEX_BUFFER(buffer, data, size) BufferSubVertexData(buffer, data, size)
#define ALICE_BUFFER_SUB_INDEX_BUFFER(buffer, data, size) BufferSubIndexData(buffer, data, size)
#define ALICE_BUFFER_SUB_UNIFORM_BUFFER(buffer, data, size) BufferUniformBufferData(buffer, data, size)
#define ALICE_BIND_GRAPHIC_PIPELINE(pipeline) BindGraphicPipeline(pipeline)
#define ALICE_BIND_VERTEX_BUFFER(buffer) BindVertexBuffer(buffer)
#define ALICE_BIND_INDEX_BUFFER(buffer,type) BindIndexBuffer(buffer,type)
#define ALICE_BIND_UNIFORM_INPUT(pipelineLayout,descriptorSet) BindUniformInput(pipelineLayout,descriptorSet)
#define ALICE_DRAW_PRIMITIVES(vertex_count) vkCmdDraw(GetMainCommandBuffer(),vertex_count,1,0,0);
#define ALICE_DRAW_PRIMITIVES_INDEXED(index_count) vkCmdDrawIndexed(GetMainCommandBuffer(), index_count, 1, 0, 0, 0)
#define ALICE_CREATE_GRAPHIC_PIPELINE_LAYOUT(pipeline_layout_create_info) CreateGraphicPipelineLayout(pipeline_layout_create_info)
#define ALICE_CREATE_GRAPHIC_PIPELINE(pipeline_create_info) CreateGraphicPipeline(pipeline_create_info)
#define ALICE_CREATE_SHADER(shaderModuleCreateInfo) CreateShaderModule(shaderModuleCreateInfo)
#define ALICE_BEGIN_RENDER_PASS(rt) BeginRendering(rt)
#define ALICE_VIEWPORT(w,h) UpdateViewport(w,h)
#define ALICE_DEPTH_BIAS(constant_factor,slope_factor) UpdateDepthBias(slope_factor,constant_factor)
#endif