#include "scene.h"
#include "ggl.h"
#include "utils.h"
#include "shader.h"
#include "ShaderPipeline.h"
#include "Material.h"
#include "model.h"
#include "SceneNode.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
#include "TextureCubeMap.h"
#include "InputAttribute.h"
#include "InputUniform.h"
#include <thread>
using namespace Alice;
static int sCanvasWidth = 0, sCanvasHeight = 0;
static SceneNode* sRootNode1 = nullptr, * sRootNode2 = nullptr;
Alice::Camera sMainCamera;
glm::mat4 model_matrix;
Alice::Model* model_sphere;
Alice::Material* material_sphere;
UniformBufferObject* sViewUniformBuffer;
void AddSceneNode(SceneNode**root, SceneNode*node) {
	if (*root==nullptr){
		*root = node;
	}
	else {
		(*root)->Append(node);
	}
}
void AddSceneNodeToRoot1(SceneNode*node) {
	AddSceneNode(&sRootNode1, node);
}
void AddSceneNodeToRoot2(SceneNode*node) {
	AddSceneNode(&sRootNode2, node);
}
void InitGeometries() {
	model_sphere = Model::LoadModel("Res/Model/Sphere.raw");
}
void InitShaders() {
	ShaderStage::CacheShaderFromPath(kShaderTypeVertex, "Test330VS", "Res/Shader/Test330.vsb");
	ShaderStage::CacheShaderFromPath(kShaderTypeFragment, "Test330FS", "Res/Shader/Test330.fsb");
}
void InitMaterials() {
}
void RenderPBRMaterialSpheresInsRGB() {
}
void Init() {
	InitGeometries();
	InitShaders();
	sViewUniformBuffer = new UniformBufferObject;
	sViewUniformBuffer->InitBuffer();
	SceneNode* node = new SceneNode;
	node->mGeometry = model_sphere;
	node->mModelMatrix = glm::translate(0.0f, 0.0f, -5.0f);
	node->mUniformInputDescription.AddUniformInput(0, kUniformInputAccessShaderStageVertex | kUniformInputAccessShaderStageFragment, kUniformTypeUniformBuffer);
	node->mUniformInputDescription.AddUniformInput(1, kUniformInputAccessShaderStageVertex | kUniformInputAccessShaderStageFragment, kUniformTypeUniformBuffer);
	material_sphere = new Alice::Material;
	material_sphere->mShaderPipeline.SetShaderStage<kShaderTypeVertex>(ShaderStage::GetShaderStage("Test330VS"));
	material_sphere->mShaderPipeline.SetShaderStage<kShaderTypeFragment>(ShaderStage::GetShaderStage("Test330FS"));
	material_sphere->mShaderPipeline.CompileShaderPipeline();
	node->mMaterial = material_sphere;
	AddSceneNodeToRoot1(node);
}
void SetViewPortSize(float width, float height) {
	sCanvasWidth = int(width);
	sCanvasHeight = int(height);
	OnVulkanViewportChanged(width, height);
	sMainCamera.mProjectionMatrix = glm::perspective(45.0f, width / height, 0.1f, 1000.0f);
	sMainCamera.mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	sViewUniformBuffer->SetMat4(0, glm::value_ptr(sMainCamera.mViewMatrix));
	sViewUniformBuffer->SetMat4(sizeof(float) * 16, glm::value_ptr(sMainCamera.mProjectionMatrix));
}
void Draw(float deltaTime) {
	float frameTime = GetFrameTime();
	sViewUniformBuffer->SyncDataFromCPUToGPU();
	ALICE_BEGIN_RENDER_PASS(0);
	ALICE_VIEWPORT(GetViewportWidth(), GetViewportHeight());
	ALICE_DEPTH_BIAS(0.0f, 0.0f);
	sRootNode1->Update(sViewUniformBuffer,deltaTime);
	sRootNode1->Render();
#if ALICE_OGL_RENDERER
	OGL_CALL(glClearColor(0.1f, 0.4f, 0.6f, 1.0f));
	OGL_CALL(glViewport(0, 0, sCanvasWidth, sCanvasHeight));
	OGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
#endif
}
void OnKeyDown(int key) {
	switch (key) {
	case 'W':
		break;
	case 'S':
		break;
	case 'D':
		break;
	case 'A':
		break;
	}
}
void OnKeyUp(int key) {

}