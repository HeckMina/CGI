#pragma once
#include "ggl.h"
#include "Geometry.h"

namespace Alice {
	class Model : public Geometry {
	public:
		void SetModelPath(const char* path);
		static Model* LoadModel(const char* path);
		static std::unordered_map<std::string, Model*> mCachedStaticMeshes;
	};
}