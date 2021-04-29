#pragma once
#include "BufferObject.h"
namespace Alice {
	enum UniformType {
		kUniformTypeUniformBuffer,
		kUniformTypeTexture2D,
		kUniformTypeTextureCubeMap,
		kUniformTypeCount
	};
	enum UniformInputAccessShaderStage {
		kUniformInputAccessShaderStageVertex = 1,
		kUniformInputAccessShaderStageTessellationControl = 2,
		kUniformInputAccessShaderStageTessellationEvaluation = 4,
		kUniformInputAccessShaderStageGeometry = 8,
		kUniformInputAccessShaderStageFragment = 16,
		kUniformInputAccessShaderStageCompute = 32
	};
	struct UniformInput {
		UniformType mType;
		int mAccessShaderStages;
		UniformInput(UniformType uniformType, int accessShaderStages):mType(uniformType),mAccessShaderStages(accessShaderStages){
		}
	};
	class UniformInputDescription {
	public:
		std::unordered_map<int, UniformInput*> mUniformInputs;
		char mName[256];
		bool mbNeedUpdate;
		UniformInputDescription();
	public:
		void AddUniformInput(int bindingPoint,int shader_stage,UniformType type);
		const char* GetName();
	};
	class UniformBufferObject : public BufferObject {
	public:
		int mBindingPoint;
		int mUniformDescriptorSetIndex;
		UniformBufferObject(int size=16384) :
			BufferObject(BufferObjectType::kBufferObjectTypeUniformBuffer, BufferUsageHint::kBufferUsageHintDynamic, size),
			mBindingPoint(-1),
			mUniformDescriptorSetIndex(-1){
			mDataBuffer = new unsigned char[size];
		}
		void SetFloat(int offset_in_bytes, const float value);
		void SetFloat2(int offset_in_bytes, const float* value);
		void SetFloat2(int offset_in_bytes, const float x, const float y);
		void SetFloat3(int offset_in_bytes, const float* value);
		void SetFloat3(int offset_in_bytes, const float x, const float y, const float z);
		void SetFloat4(int offset_in_bytes, const float* value);
		void SetFloat4(int offset_in_bytes, const float x, const float y, const float z, const float w);
		void SetMat4(int offset_in_bytes, const float* value);
	};
	class UniformInputLayoutDescriptor;
	class UniformInputLayoutInstance {
	public:
		std::vector<VkWriteDescriptorSet> mWriteDescriptorSet;
		VkDescriptorSet mDescriptorSet;
		UniformInputLayoutDescriptor* mUniformInputLayoutDescriptor;
		bool mbNeedUpdate;
	public:
		void Init(const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		int BindingTexture(int bindingPoint, Texture*texture);
		int BindingUniformBuffer(int bindingPoint, UniformBufferObject* uniformBuffer);
		bool IsTextureBinded(int bindingPoint,Texture* texture);
		bool IsUniformBufferBinded(int bindingPoint, UniformBufferObject* uniformBuffer);
		void Update();
	};
	class UniformInputLayoutDescriptor {
	public:
		std::vector<VkDescriptorSetLayoutBinding> mDescriptorSetLayoutBindings;
		std::vector<VkDescriptorPoolSize> mDescriptorPoolSize;
		VkDescriptorSetLayout mDescriptorSetLayout;
		VkDescriptorPool mDescriptorPool;
	public:
		static void InitDefaultUniformInputType();
		static UniformInputLayoutInstance* CreateUniformInputLayoutInstance(const char* name);
		static void Register(UniformInputDescription* uniformInputDescription);
		static void CleanUp();
	private:
		void AddTexture(int bindingPoint, VkShaderStageFlags shader_stage);
		void AddUniformBuffer(int bindingPoint, VkShaderStageFlags shader_stage);
		static std::unordered_map<std::string, UniformInputLayoutDescriptor*> sUniformInputLayoutDescriptors;
	};
}