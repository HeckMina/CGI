#include "utils.h"
#include "stbi/stb_image_aug.h"
void SwapRGBPixel(unsigned char * pixel, int src0, int src1) {
	int src0_data_offset = src0 * 3;
	int src1_data_offset = src1 * 3;
	unsigned char src1_pixel_r = pixel[src1_data_offset];
	unsigned char src1_pixel_g = pixel[src1_data_offset + 1];
	unsigned char src1_pixel_b = pixel[src1_data_offset + 2];
	pixel[src1_data_offset] = pixel[src0_data_offset];
	pixel[src1_data_offset + 1] = pixel[src0_data_offset + 1];
	pixel[src1_data_offset + 2] = pixel[src0_data_offset + 2];
	pixel[src0_data_offset] = src1_pixel_r;
	pixel[src0_data_offset + 1] = src1_pixel_g;
	pixel[src0_data_offset + 2] = src1_pixel_b;
}
void SwapRGB32FPixel(float * pixel, int src0, int src1) {
	int src0_data_offset = src0 * 3;
	int src1_data_offset = src1 * 3;
	float src1_pixel_r = pixel[src1_data_offset];
	float src1_pixel_g = pixel[src1_data_offset + 1];
	float src1_pixel_b = pixel[src1_data_offset + 2];
	pixel[src1_data_offset] = pixel[src0_data_offset];
	pixel[src1_data_offset + 1] = pixel[src0_data_offset + 1];
	pixel[src1_data_offset + 2] = pixel[src0_data_offset + 2];
	pixel[src0_data_offset] = src1_pixel_r;
	pixel[src0_data_offset + 1] = src1_pixel_g;
	pixel[src0_data_offset + 2] = src1_pixel_b;
}
void SwapRGBAPixel(unsigned char * pixel, int src0, int src1) {
	int src0_data_offset = src0 * 4;
	int src1_data_offset = src1 * 4;
	unsigned char src1_pixel_r = pixel[src1_data_offset];
	unsigned char src1_pixel_g = pixel[src1_data_offset + 1];
	unsigned char src1_pixel_b = pixel[src1_data_offset + 2];
	unsigned char src1_pixel_a = pixel[src1_data_offset + 3];
	pixel[src1_data_offset] = pixel[src0_data_offset];
	pixel[src1_data_offset + 1] = pixel[src0_data_offset + 1];
	pixel[src1_data_offset + 2] = pixel[src0_data_offset + 2];
	pixel[src1_data_offset + 3] = pixel[src0_data_offset + 3];
	pixel[src0_data_offset] = src1_pixel_r;
	pixel[src0_data_offset + 1] = src1_pixel_g;
	pixel[src0_data_offset + 2] = src1_pixel_b;
	pixel[src0_data_offset + 3] = src1_pixel_a;
}
void FlipImage(unsigned char*pixel_data, int width, int height, int channel_count) {
	int half_height = height / 2;
	for (int y = 0; y < half_height; ++y) {
		for (int x = 0; x < width; ++x) {
			int src_pixel_index = y * width + x;
			int dst_pixel_index = (height - y - 1)*width + x;
			if (channel_count == 3) {
				SwapRGBPixel(pixel_data, src_pixel_index, dst_pixel_index);
			}
			else if (channel_count == 4) {
				SwapRGBAPixel(pixel_data, src_pixel_index, dst_pixel_index);
			}
		}
	}
}
void FlipImage32F(float*pixel_data, int width, int height, int channel_count) {
	int half_height = height / 2;
	for (int y = 0; y < half_height; ++y) {
		for (int x = 0; x < width; ++x) {
			int src_pixel_index = y * width + x;
			int dst_pixel_index = (height - y - 1)*width + x;
			if (channel_count == 3) {
				SwapRGB32FPixel(pixel_data, src_pixel_index, dst_pixel_index);
			}
		}
	}
}
void FlipImageX(unsigned char*pixel_data, int width, int height, int channel_count) {
	int half_width = width / 2;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < half_width; ++x) {
			int src_pixel_index = y * width + x;
			int dst_pixel_index = y * width + width -1 - x;
			if (channel_count == 3) {
				SwapRGBPixel(pixel_data, src_pixel_index, dst_pixel_index);
			}
			else if (channel_count == 4) {
				SwapRGBAPixel(pixel_data, src_pixel_index, dst_pixel_index);
			}
		}
	}
}
void SwapRB(unsigned char * pixel, int pixel_data_offset) {
	unsigned char temp = pixel[pixel_data_offset];
	pixel[pixel_data_offset] = pixel[pixel_data_offset + 2];
	pixel[pixel_data_offset + 2] = temp;
}
unsigned char* DecodeBMP(unsigned char*bmpFileData, int&width, int&height) {
	if (0x4D42 == *((unsigned short*)bmpFileData)) {
		int pixelDataOffset = *((int*)(bmpFileData + 10));
		width = *((int*)(bmpFileData + 18));
		height = *((int*)(bmpFileData + 22));
		unsigned char*pixelData = bmpFileData + pixelDataOffset;
		for (int i = 0; i < width*height;++i) {
			SwapRB(pixelData, i*3);
		}
		return pixelData;
	}
	return nullptr;
}
void RGBAImageToRGBImage(unsigned char * rgba_pixel, unsigned char * rgb_pixel, int width, int height) {
	for (int i = 0; i < width*height; ++i) {
		rgb_pixel[i * 3] = rgba_pixel[i * 4];
		rgb_pixel[i * 3 + 1] = rgba_pixel[i * 4 + 1];
		rgb_pixel[i * 3 + 2] = rgba_pixel[i * 4 + 2];
	}
}
void SaveRGBToBMP(const char * path,unsigned char * pixel,int width, int height){
	FILE*pFile = FOPEN(path, "wb");
	if (pFile){
		BITMAPFILEHEADER bfh;
		memset(&bfh, 0, sizeof(BITMAPFILEHEADER));
		bfh.bfType = 0x4D42;
		bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + width * height * 3;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pFile);

		BITMAPINFOHEADER bih;
		memset(&bih, 0, sizeof(BITMAPINFOHEADER));
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biBitCount = 24;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pFile);
		//rgb -> bgr
		for (int i = 0; i < width*height; ++i) {
			SwapRB(pixel, i*3);
		}
		fwrite(pixel, 1, width*height * 3, pFile);
		fclose(pFile);
	}
}
void SaveRGBAToBMP(const char * path, unsigned char * pixel, int width, int height) {
	unsigned char * rgb_pixel = new unsigned char[width*height*3];
	RGBAImageToRGBImage(pixel, rgb_pixel, width, height);
	SaveRGBToBMP(path, rgb_pixel, width, height);
	delete[]rgb_pixel;
}