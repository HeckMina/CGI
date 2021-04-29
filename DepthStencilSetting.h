#pragma once
namespace Alice {
	enum StencilOperation {
		kStencilOperationKeep,
		kStencilOperationZero,
		kStencilOperationReplace,
		kStencilOperationInrementAndClamp,
		kStencilOperationDecrementAndClamp,
		kStencilOperationInvert,
		kStencilOperationIncrementAndWrap,
		kStencilOperationDecrementAndWrap,
		kStencilOperationCount
	};
	enum CompareOperation {
		kCompareOperationNever,
		kCompareOperationLess,
		kCompareOperationEqual,
		kCompareOperationLessOrEqual,
		kCompareOperationGreater,
		kCompareOperationNotEqual,
		kCompareOperationGreaterOrEqual,
		kCompareOperationAlways,
		kCompareOperationCount
	};
	class StencilSetting {
	public:
		StencilOperation mFailedOperation, mPassOperation, mDepthFailedOperation;
		CompareOperation mCompareOperation;
		unsigned int mCompareMask, mWriteMask, mReferenceValue;
	};
	class DepthStencilSetting {
	public:
		bool mbEnableDepthTest;
		bool mbEnableDepthWrite;
		CompareOperation mDepthCompareOperation;
		bool mbEnableDepthBoundsTest;
		float mMinDepthBounds;
		float mMaxDepthBounds;
		bool mbEnableStencilTest;
		StencilSetting mFrontFaceStencilSetting, mBackFaceStencilSetting;
		DepthStencilSetting() :mbEnableDepthTest(true),mbEnableDepthWrite(true),mDepthCompareOperation(kCompareOperationLessOrEqual),
			mbEnableDepthBoundsTest(false),mMinDepthBounds(0.0f),mMaxDepthBounds(1.0f),mbEnableStencilTest(false){

		}
	};
}