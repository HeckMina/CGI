#pragma once
namespace Alice {
	enum MultiSampleCount {
		kMultiSampleCount_x1,
		kMultiSampleCount_x2,
		kMultiSampleCount_x4,
		kMultiSampleCount_x8,
		kMultiSampleCount_x16,
		kMultiSampleCount_x32,
		kMultiSampleCount_x64
	};
	class MultiSampleSetting {
	public:
		bool mbEnableSampleShading;
		bool mbEnableAlphaToCoverage;
		bool mbEnableAlphaToOne;
		MultiSampleCount mSampleCount;
		float mMinSampleShading;
		unsigned int* mSampleMask;
		MultiSampleSetting() :mbEnableSampleShading(true), mbEnableAlphaToCoverage(false), mbEnableAlphaToOne(false),
			mSampleCount(kMultiSampleCount_x1),mSampleMask(nullptr), mMinSampleShading(1.0f) {

		}
	};
}