#include "ShaderPipeline.h"
#include "utils.h"
#include "CGI.h"
namespace Alice {
#ifdef ALICE_OGL_RENDERER
	GLuint CreateShader(GLenum shaderType, const char* shaderCode, int shader_code_length) {
		GLuint shader = 0;
		OGL_CALL(shader = glCreateShader(shaderType));
		OGL_CALL(glShaderSource(shader, 1, &shaderCode, nullptr));
		OGL_CALL(glCompileShader(shader));
		GLint compileResult = GL_TRUE;
		OGL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult));
		if (compileResult == GL_FALSE) {
			char szLog[10240] = { 0 };
			GLsizei logLen = 0;
			OGL_CALL(glGetShaderInfoLog(shader, 10240, &logLen, szLog));
			printf("Compile Shader Failed\nError Log: %s \nshader code :\n%s\n", szLog, shaderCode);
			OGL_CALL(glDeleteShader(shader));
			shader = 0;
		}
		return shader;
	}
#else
	VkShaderModule CreateShader(const char* shaderCode, int shader_code_length) {
		ALICE_SHADER shader;
		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = shader_code_length;
		shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode;
		shader = ALICE_CREATE_SHADER(shaderModuleCreateInfo);
		return shader;
	}
#endif
	std::unordered_map<std::string, ShaderStage*> ShaderStage::mCachedShaders;
	void ShaderStage::CacheShader(ShaderType shader_type, const char* shader_name, const char* shader_code, int shader_code_length) {
#ifdef ALICE_OGL_RENDERER
		GLenum cgi_shader_type = GL_VERTEX_SHADER;
		switch (shader_type) {
		case kShaderTypeVertex:
			cgi_shader_type = GL_VERTEX_SHADER;
			break;
		case kShaderTypeTessellationControl:
			cgi_shader_type = GL_TESS_CONTROL_SHADER;
			break;
		case kShaderTypeTessellationEvaluation:
			cgi_shader_type = GL_TESS_EVALUATION_SHADER;
			break;
		case kShaderTypeGeometry:
			cgi_shader_type = GL_GEOMETRY_SHADER;
			break;
		case kShaderTypeFragment:
			cgi_shader_type = GL_FRAGMENT_SHADER;
			break;
		case kShaderTypeCompute:
			break;
		}
		ALICE_SHADER cgi_shader = CreateShader(cgi_shader_type, shader_code, shader_code_length);
#elif ALICE_VULKAN_RENDERER
		ALICE_SHADER cgi_shader = CreateShader(shader_code, shader_code_length);
#endif
		if (cgi_shader==0){
			return;
		}
		printf("Cache Shader [%s] Success!\n",shader_name);
		mCachedShaders.insert(std::pair<std::string, ShaderStage*>(shader_name, new ShaderStage(shader_name, shader_type, cgi_shader)));
	}
	void ShaderStage::CacheShaderFromPath(ShaderType shader_type, const char* shader_name, const char* path) {
		int shader_code_length = 0;
		unsigned char* shader_code = LoadFileContent(path,shader_code_length);
		CacheShader(shader_type, shader_name, (char*)shader_code, shader_code_length);
	}
	ShaderStage* ShaderStage::GetShaderStage(const char* shader_name) {
		auto iter = mCachedShaders.find(shader_name);
		if (iter != mCachedShaders.end()) {
			return iter->second;
		}
		return nullptr;
	}
#if ALICE_OGL_RENDERER
	void ShaderPipeline::CompileShaderPipeline() {
		OGL_CALL(mProgram = glCreateProgram());
		for (int i = 0; i < 5; ++i) {
			if (mShaderStages[i] != nullptr) {
				OGL_CALL(glAttachShader(mProgram, mShaderStages[i]->GetCGIShader()));
			}
		}
		OGL_CALL(glLinkProgram(mProgram));
		for (int i = 0; i < 5; ++i) {
			if (mShaderStages[i] != nullptr) {
				OGL_CALL(glDetachShader(mProgram, mShaderStages[i]->GetCGIShader()));
			}
		}
		GLint nResult;
		OGL_CALL(glGetProgramiv(mProgram, GL_LINK_STATUS, &nResult));
		if (nResult == GL_FALSE) {
			char log[10240] = { 0 };
			GLsizei writed = 0;
			OGL_CALL(glGetProgramInfoLog(mProgram, 10240, &writed, log));
			printf("Create Program Failed\n[%s][%s][%s][%s][%s]\nlink error : %s\n",
				mShaderStages[0] == nullptr ? "None" : mShaderStages[0]->GetName(), mShaderStages[1] == nullptr ? "None" : mShaderStages[1]->GetName(),
				mShaderStages[2] == nullptr ? "None" : mShaderStages[2]->GetName(), mShaderStages[3] == nullptr ? "None" : mShaderStages[3]->GetName(),
				mShaderStages[4] == nullptr ? "None" : mShaderStages[4]->GetName(), log);
			OGL_CALL(glDeleteProgram(mProgram));
			mProgram = 0;
		}
		else {
			printf("CompileShaderPipeline Sucess!\n");
		}
	}
#endif
}