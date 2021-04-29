#pragma once
#if ALICE_OGL_RENDERER
#include "ggl.h"
GLuint CompileShader(GLenum shaderType, const char* shaderCode);
GLuint CreateProgram(GLuint vsShader, GLuint fsShader);
GLuint CreateTexture2D(unsigned char* pixelData, int width, int height, GLenum gpu_format = GL_RGB, GLenum cpu_format = GL_RGB, GLenum date_type = GL_UNSIGNED_BYTE);
GLuint CreateTextureFromFile(const char* path);
GLuint InitTextureCube(int size, GLenum gpu_format, GLenum data_type);
GLuint CreateTextureCubeFromBMP(const char** images, bool correct_with_shader = false);
GLuint CreateBufferObject(GLenum bufferType, GLsizeiptr size, GLenum usage, void* data = nullptr);
#define GenVertexBuffer(buffer,size,usage) do { \
		buffer = CreateBufferObject(GL_ARRAY_BUFFER, size, usage); \
	} while (0)
#define GenIndexBuffer(buffer,size,usage) do { \
		buffer = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, size, usage); \
	} while (0)
#define GenUniformBuffer(buffer,size,usage) do { \
		buffer = 0; \
	} while (0)
void UpdateBufferObject(GLuint object, GLenum type, void* data, int size, int offset);
#define BufferSubVertexData(buffer,data,size) \
	UpdateBufferObject(buffer, GL_ARRAY_BUFFER, data, size, 0);
#define BufferSubIndexData(buffer,data,size) \
	UpdateBufferObject(buffer, GL_ELEMENT_ARRAY_BUFFER, data, size, 0);
#define BufferUniformBufferData(buffer,data,size) 
void CheckLastOpenGLError(const char* prefix, const char* file, long line, const char* operation);
#define GLAssert(x) 	{ CheckLastOpenGLError (NULL,__FILE__, __LINE__,#x); }
#define OGL_CALL(x) do { x; GLAssert(x); } while(0)
#endif