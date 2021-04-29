#pragma once
#include "MultiSampleSetting.h"
#include "CGI.h"
namespace Alice {
	class BlendSetting {
	public:
		bool mbEnableBlend;
		unsigned int mColorMask;
		BlendOperation mColorBlendOperation, mAlphaBlendOperation;
		BlendFactor mSrcColorBlendFactor, mSrcAlphaBlendFactor;
		BlendFactor mDstColorBlendFactor, mDstAlphaBlendFactor;
	};
	class ColorBlendSetting {
	public:
		BlendSetting mBlendSetting[8];
		int mColorAttachmentCount;
		bool mbEnableLogicOperation;
		LogicOperation mLogicOperation;
		float mBlendConstants[4];
		ColorBlendSetting() : mColorAttachmentCount(0) {}
		void AppendColorBlendSetting(bool enable_blend, unsigned int color_mask, BlendOperation color_op, BlendOperation alpha_op,
			BlendFactor src_color, BlendFactor src_alpha, BlendFactor dst_color, BlendFactor dst_alpha) {
			mBlendSetting[mColorAttachmentCount].mbEnableBlend = enable_blend;
			mBlendSetting[mColorAttachmentCount].mColorMask = color_mask;
			mBlendSetting[mColorAttachmentCount].mColorBlendOperation = color_op;
			mBlendSetting[mColorAttachmentCount].mAlphaBlendOperation = alpha_op;
			mBlendSetting[mColorAttachmentCount].mSrcColorBlendFactor = src_color;
			mBlendSetting[mColorAttachmentCount].mSrcAlphaBlendFactor = src_alpha;
			mBlendSetting[mColorAttachmentCount].mDstColorBlendFactor = dst_color;
			mBlendSetting[mColorAttachmentCount++].mDstAlphaBlendFactor = dst_alpha;
		}
	};
	enum CGPassType {
		kCGPassTypePreZ,
		kCGPassTypeBasePass,
		kCGPassTypeAdditive,
		kCGPassTypeGBuffer,
		kCGPassTypeTranslucency,
		kCGPassTypePostProcessing
	};
	enum RenderTargetType {
		kRenderTargetTypeColor0,
		kRenderTargetTypeColor1,
		kRenderTargetTypeColor2,
		kRenderTargetTypeColor3,
		kRenderTargetTypeColor4,
		kRenderTargetTypeColor5,
		kRenderTargetTypeColor6,
		kRenderTargetTypeColor7,
		kRenderTargetTypeDepthStencil,
		kRenderTargetTypeCount
	};
	enum RenderTargetOperationOnLoad {
		kRenderTargetOperationOnLoadLoad,
		kRenderTargetOperationOnLoadClear,
		kRenderTargetOperationOnLoadDontCare
	};
	enum RenderTargetOperationOnStore {
		kRenderTargetOperationOnStoreStore,
		kRenderTargetOperationOnStoreDontCare,
		kRenderTargetOperationOnStoreNoneQcom
	};
	enum ImageFormat{
		kImageFormatR8G8B8A8,
		kImageFormatD24S8
	};
	enum ResourceLayout {
		kResourceLayoutUndefined,
		kResourceLayoutGeneral,
		kResourceLayoutColorAttachmentOptimal,
		kResourceLayoutDepthStencilAttachmentOptimal,
		kResourceLayoutDepthStencilReadOnlyOptimal,
		kResourceLayoutShaderReadOnlyOptimal,
		kResourceLayoutTransferSrcOptimal,
		kResourceLayoutTransferDstOptimal,
		kResourceLayoutPreInitialized
	};
	class RenderTargetSetting {
	public:
		ImageFormat mFormat;
		MultiSampleCount mSampleCount;
		RenderTargetOperationOnLoad mOperationOnLoad;
		RenderTargetOperationOnStore mOperationOnStore;
		RenderTargetOperationOnLoad mStencilOperationOnLoad;
		RenderTargetOperationOnStore mStencilOperationOnStore;
		ResourceLayout mInitialLayout;
		ResourceLayout mFinalLayout;
	};
	class CGRenderPass {
	public:
#if ALICE_VULKAN_RENDERER
		VkRenderPass mRenderPass;
#endif
		RenderTargetSetting mRenderTargetSettings[8];
		int mRenderTargetCount;
		CGRenderPass() :mRenderTargetCount(0) {
		}
		void SetRenderTargetSetting(int index, ImageFormat format, MultiSampleCount sampleCount, RenderTargetOperationOnLoad op_on_load,
			RenderTargetOperationOnStore op_on_store, RenderTargetOperationOnLoad stencil_op_on_load,RenderTargetOperationOnStore stencil_op_on_store, 
			ResourceLayout initialLayout, ResourceLayout finalLayout) {
			mRenderTargetSettings[index].mFormat = format;
			mRenderTargetSettings[index].mSampleCount = sampleCount;
			mRenderTargetSettings[index].mOperationOnLoad = op_on_load;
			mRenderTargetSettings[index].mOperationOnStore = op_on_store;
			mRenderTargetSettings[index].mStencilOperationOnLoad = stencil_op_on_load;
			mRenderTargetSettings[index].mStencilOperationOnStore = stencil_op_on_store;
			mRenderTargetSettings[index].mInitialLayout = initialLayout;
			mRenderTargetSettings[index].mFinalLayout = finalLayout;
			mRenderTargetCount = mRenderTargetCount - 1 < index ? index + 1 : mRenderTargetCount;
		}
		bool Compile();
	};
	class RenderTarget {
	public:
	};
}