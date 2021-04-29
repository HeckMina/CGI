#include "model.h"
#include "utils.h"
namespace Alice {
	std::unordered_map<std::string, Model*> Model::mCachedStaticMeshes;
	void Model::SetModelPath(const char* path) {
		FILE* file = FOPEN(path, "rb");
		if (file != NULL) {
			int vertice_count;
			fread(&vertice_count, 1, sizeof(int), file);
			SetVertexCount(vertice_count);
			fread(mVBO->mDataBuffer, 1, sizeof(Vertex) * vertice_count, file);
			fread(&mIndexCount, 1, sizeof(int), file);
			SetIndexCount(mIndexCount);
			fread(mIBO->mDataBuffer, 1, sizeof(unsigned int) * mIndexCount, file);
			fclose(file);
		}
		Submit();
	}
	Model* Model::LoadModel(const char* path) {
		auto iter = mCachedStaticMeshes.find(path);
		if (iter!=mCachedStaticMeshes.end()){
			return iter->second;
		}
		Model* model = new Model;
		model->SetModelPath(path);
		mCachedStaticMeshes.insert(std::pair<std::string, Model*>(path,model));
		return model;
	}
}