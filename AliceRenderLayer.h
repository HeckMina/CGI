#pragma once
#include <windows.h>
#ifdef ALICE_OGL_RENDERER
#include "glew.h"
#include "wglew.h"
#elif ALICE_VULKAN_RENDERER
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#endif
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <map>
#include <unordered_map>
#include "Glm/glm.hpp"
#include "Glm/ext.hpp"
#define RAW_C_ARRAY_ELEMENT_COUNT(a) (sizeof(a)/sizeof(a[0]))
namespace Alice {
	struct Vertex {
		float mPosition[4];
		float mTexcoord[4];
		float mNormal[4];
		float mTangent[4];
	};
	enum class BufferUsage {
		kBufferUsageStatic,
		kBufferUsageDynamic,
		kBufferUsageCount
	};
}