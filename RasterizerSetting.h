#pragma once
namespace Alice {
	enum PolygonMode {
		kPolygonModeFill,
		kPolygonModeLine,
		kPolygonModePoint,
		kPolygonModeCount
	};
	enum FaceCullMode {
		kFaceCullModeNone,
		kFaceCullModeFront,
		kFaceCullModeBack,
		kFaceCullModeFrontAndBack,
		kFaceCullModeCount
	};
	enum FrontFace {
		kFrontFaceCCW,
		kFrontFaceCW,
		kFrontFaceCount
	};
	class RasterizerSetting {
	public:
		PolygonMode mPolygonMode;
		float mLineWidth;
		bool mbEnableClampDepth;
		bool mbEnableRasterizerDiscard;
		FaceCullMode mFaceCullMode;
		FrontFace mFrontFace;
		bool mbEnableDepthBias;
		float mDepthBiasConstantFactor;
		float mDepthBiasClamp;
		float mDepthBiasSlopeFactor;
		RasterizerSetting():mPolygonMode(kPolygonModeFill),mLineWidth(1.0f),mbEnableClampDepth(false),mbEnableRasterizerDiscard(false),
			mFaceCullMode(kFaceCullModeBack),mFrontFace(kFrontFaceCCW),mbEnableDepthBias(false),mDepthBiasConstantFactor(0.0f),mDepthBiasClamp(1.0f),
			mDepthBiasSlopeFactor(0.0f){
		}
	};
}