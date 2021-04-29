#pragma once
#include "ggl.h"
#ifdef ALICE_OGL_RENDERER
#define ALICE_SHADER GLuint
#elif ALICE_VULKAN_RENDERER
#define ALICE_SHADER VkShaderModule
#endif
namespace Alice {
	enum ShaderType {
		kShaderTypeVertex,
		kShaderTypeTessellationControl,
		kShaderTypeTessellationEvaluation,
		kShaderTypeGeometry,
		kShaderTypeFragment,
		kShaderTypeCompute,
		kShaderTypeCount
	};
	class ShaderStage {
	protected:
		ShaderType mType;
		ALICE_SHADER mShader;
		char mName[256];//for debug info
	public:
		ShaderStage(const char* name, ShaderType type, ALICE_SHADER shader) :mType(type), mShader(shader) {
			memset(mName, 0, 256);
			strcpy(mName, name);
		}
		ALICE_SHADER GetCGIShader() { return mShader; }
		const char* GetName() { return mName; }
		ShaderType GetType() { return mType; }
	public:
		static void CacheShader(ShaderType shader_type, const char* shader_name, const char* shader_code, int shader_code_length);
		static void CacheShaderFromPath(ShaderType shader_type, const char* shader_name, const char* path);
		static ShaderStage* GetShaderStage(const char* shader_name);
		static std::unordered_map<std::string, ShaderStage*> mCachedShaders;
	};
	class ShaderPipeline {
	public:
		ShaderStage* mShaderStages[5];
		ShaderPipeline() {
			mShaderStages[0] = nullptr;
			mShaderStages[1] = nullptr;
			mShaderStages[2] = nullptr;
			mShaderStages[3] = nullptr;
			mShaderStages[4] = nullptr;
#if ALICE_OGL_RENDERER
			mProgram = 0;
#endif
		}
		template<ShaderType type>
		void SetShaderStage(ShaderStage* shader_stage) {
			mShaderStages[(int)type] = shader_stage;
		}
#if ALICE_OGL_RENDERER
		GLuint mProgram;
		void CompileShaderPipeline();
#elif ALICE_VULKAN_RENDERER
		void CompileShaderPipeline() {}
#endif
	};
}