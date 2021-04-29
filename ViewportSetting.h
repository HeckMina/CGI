#pragma once
namespace Alice {
	class ScissorSetting {
	public:
		int mOffsetX, mOffsetY;
		unsigned int mWidth, mHeight;
	};
	class ViewportSetting {
	public:
		float mX, mY, mWidth, mHeight, mMinDepth, mMaxDepth;
		ScissorSetting mScissorSetting;
	};
}