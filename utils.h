#pragma once
#include "ggl.h"
namespace Alice {
	class Camera {
	public:
		glm::mat4 mViewMatrix;
		glm::mat4 mProjectionMatrix;
	};
}
float GetFrameTime();
unsigned char * LoadFileContent(const char*path, int&filesize);
unsigned char * DecodeBMP(unsigned char*bmpFileData, int&width, int&height);
void SaveRGBToBMP(const char * path, unsigned char * pixel, int width, int height);
void SaveRGBAToBMP(const char * path, unsigned char * pixel, int width, int height);
void FlipImage32F(float*pixel_data, int width, int height, int channel_count);
__inline FILE* Alicefopen(const char* path, const char* mode) {
	FILE* temp = nullptr;
	return fopen_s(&temp, path, mode) == 0 ? temp : nullptr;
}
#define FOPEN(path,mode) Alicefopen(path,mode)