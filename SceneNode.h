#pragma once
#include "utils.h"
#include "GraphicPipeline.h"
namespace Alice {
	class Geometry;
	class Material;
	class Camera;
	struct UniformInput;
	class LinkedList {
	public:
		LinkedList* mNext;
		LinkedList() :mNext(nullptr) {}
		void Append(LinkedList* node) {
			if (mNext == nullptr) {
				mNext = node;
			}
			else {
				mNext->Append(node);
			}
		}
		template<typename T>
		T* Next() {
			return (T*)mNext;
		}
	};
	class SceneNode : public LinkedList {
	public:
		glm::mat4 mModelMatrix;
		Geometry* mGeometry;
		Material* mMaterial;
		GraphicPipeline* mGraphicPipeline;
		UniformInput *mPrimitiveUniform;
		UniformInputDescription mUniformInputDescription;
		UniformInputLayoutInstance* mUniformInputLayoutInstance;
		SceneNode();
		void Update(UniformBufferObject* viewUniformBuffer,float delta);
		void Render();
	};
}