#include "InputUniform.h"
namespace Alice {
	void UniformBufferObject::SetFloat(int offset_in_bytes, const float value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = value;
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetFloat2(int offset_in_bytes, const float x, const float y) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = x;
		*(dataPtr + 1) = y;
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetFloat2(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 2);
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetFloat3(int offset_in_bytes, const float x, const float y, const float z) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = x;
		*(dataPtr + 1) = y;
		*(dataPtr + 2) = z;
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetFloat3(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 3);
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetFloat4(int offset_in_bytes, const float x, const float y, const float z, const float w) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		*dataPtr = x;
		*(dataPtr + 1) = y;
		*(dataPtr + 2) = z;
		*(dataPtr + 3) = w;
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetFloat4(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 4);
		mbNeedSyncToGPU = true;
	}
	void UniformBufferObject::SetMat4(int offset_in_bytes, const float* value) {
		float* dataPtr = (float*)((char*)mDataBuffer + offset_in_bytes);
		memcpy(dataPtr, value, sizeof(float) * 16);
		mbNeedSyncToGPU = true;
	}
	UniformInputDescription::UniformInputDescription() {
		mbNeedUpdate = false;
	}
	void UniformInputDescription::AddUniformInput(int bindingPoint, int shader_stage, UniformType type) {
		UniformInput* uniform_input = new UniformInput(type, shader_stage);
		mUniformInputs.insert(std::pair<int, UniformInput*>(bindingPoint,uniform_input));
		mbNeedUpdate = true;
	}
	const char* UniformInputDescription::GetName() {
		if (mbNeedUpdate){
			mbNeedUpdate = false;
			memset(mName, 0, 256);
			char temp[256] = { 0 };
			int index = 0;
			for (auto iter=mUniformInputs.begin();iter!=mUniformInputs.end();++iter){
				memset(temp, 0, 256);
				if (iter->second->mType == kUniformTypeUniformBuffer) {
					sprintf(temp, "%dU", iter->first);
				}
				else {
					sprintf(temp, "%dT", iter->first);
				}
				strcpy(mName+index, temp);
				index = strlen(mName);
			}	
		}
		return mName;
	}
	void UniformInputLayoutInstance::Init(const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings) {
		for (int i=0;i<descriptorSetLayoutBindings.size();++i){
			const VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayoutBindings[i];
			VkWriteDescriptorSet writeDescriptorSet = {};
			writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet.dstSet = mDescriptorSet;
			writeDescriptorSet.dstBinding = descriptorSetLayoutBinding.binding;
			writeDescriptorSet.dstArrayElement = 0;
			writeDescriptorSet.descriptorType = descriptorSetLayoutBinding.descriptorType;
			writeDescriptorSet.descriptorCount = 1;
			mWriteDescriptorSet.push_back(writeDescriptorSet);
		}
	}
	int UniformInputLayoutInstance::BindingTexture(int bindingPoint, Texture* texture) {
		for (int i = 0; i < mWriteDescriptorSet.size(); ++i) {
			VkWriteDescriptorSet& descriptorSet = mWriteDescriptorSet[i];
			if (descriptorSet.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
				descriptorSet.dstBinding == bindingPoint) {
				VkDescriptorImageInfo* descriptorImageInfo = new VkDescriptorImageInfo;
				descriptorImageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				descriptorImageInfo->imageView = texture->mImageView;
				descriptorImageInfo->sampler = texture->mSampler;
				descriptorSet.pImageInfo = descriptorImageInfo;
				return i;
			}
		}
		return -1;
	}
	int UniformInputLayoutInstance::BindingUniformBuffer(int bindingPoint, UniformBufferObject* uniformBuffer) {
		for (int i = 0; i < mWriteDescriptorSet.size(); ++i) {
			VkWriteDescriptorSet& descriptorSet = mWriteDescriptorSet[i];
			if (descriptorSet.descriptorType==VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER&&
				descriptorSet.dstBinding==bindingPoint){
				if (descriptorSet.pBufferInfo!=nullptr&&descriptorSet.pBufferInfo->buffer==uniformBuffer->mBuffer){
					return -2;
				}
				VkDescriptorBufferInfo* bufferinfo = new VkDescriptorBufferInfo;
				bufferinfo->offset = 0;
				bufferinfo->buffer = uniformBuffer->mBuffer;
				bufferinfo->range = uniformBuffer->GetBufferSize();
				descriptorSet.pBufferInfo = bufferinfo;
				uniformBuffer->mBindingPoint = bindingPoint;
				uniformBuffer->mUniformDescriptorSetIndex = i;
				mbNeedUpdate = false;
				return i;
			}
		}
		return -1;
	}
	bool UniformInputLayoutInstance::IsUniformBufferBinded(int bindingPoint, UniformBufferObject* uniformBuffer) {
		if (uniformBuffer->mUniformDescriptorSetIndex == -1) {
			return false;
		}
		if (uniformBuffer->mUniformDescriptorSetIndex >= mWriteDescriptorSet.size()) {
			return false;
		}
		if (mWriteDescriptorSet[uniformBuffer->mUniformDescriptorSetIndex].pBufferInfo->buffer!=uniformBuffer->mBuffer){
			return false;
		}
		return true;
	}
	bool UniformInputLayoutInstance::IsTextureBinded(int bindingPoint, Texture* texture) {
		/*if (texture->m == -1) {
			return false;
		}
		if (texture->mUniformDescriptorSetIndex >= mWriteDescriptorSet.size()) {
			return false;
		}
		if (mWriteDescriptorSet[texture->mUniformDescriptorSetIndex].pBufferInfo->buffer != texture->mBuffer) {
			return false;
		}*/
		return true;
	}
	void UniformInputLayoutInstance::Update() {
		UpdateDescriptorSets(mWriteDescriptorSet);
	}
	std::unordered_map<std::string, UniformInputLayoutDescriptor*> UniformInputLayoutDescriptor::sUniformInputLayoutDescriptors;
	void UniformInputLayoutDescriptor::InitDefaultUniformInputType() {
		//ES2.0 specification

	}
	UniformInputLayoutInstance* UniformInputLayoutDescriptor::CreateUniformInputLayoutInstance(const char* name) {
		auto iter = sUniformInputLayoutDescriptors.find(name);
		if (iter == sUniformInputLayoutDescriptors.end()) {
			return nullptr;
		}
		UniformInputLayoutInstance* uniformInputLayoutInstance = new UniformInputLayoutInstance;
		uniformInputLayoutInstance->mUniformInputLayoutDescriptor = iter->second;
		uniformInputLayoutInstance->mDescriptorSet = CreateDescriptorSet(iter->second->mDescriptorSetLayout, iter->second->mDescriptorPool);
		uniformInputLayoutInstance->Init(iter->second->mDescriptorSetLayoutBindings);
		return uniformInputLayoutInstance;
	}
	void UniformInputLayoutDescriptor::CleanUp() {

	}
	void UniformInputLayoutDescriptor::Register(UniformInputDescription* uniformInputDescription) {
		auto iter = sUniformInputLayoutDescriptors.find(uniformInputDescription->GetName());
		if (iter!=sUniformInputLayoutDescriptors.end()){
			return;
		}
		UniformInputLayoutDescriptor* uniformInputLayoutDescriptor = new UniformInputLayoutDescriptor;
		for (auto iter = uniformInputDescription->mUniformInputs.begin(); iter != uniformInputDescription->mUniformInputs.end(); ++iter) {
			int accessShaderStage = iter->second->mAccessShaderStages;
			VkFlags shader_stages = 0;
			if ((accessShaderStage&kUniformInputAccessShaderStageVertex)!=0){
				shader_stages |= VK_SHADER_STAGE_VERTEX_BIT;
			}
			if ((accessShaderStage & kUniformInputAccessShaderStageFragment) != 0) {
				shader_stages |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			if (iter->second->mType == kUniformTypeUniformBuffer) {
				uniformInputLayoutDescriptor->AddUniformBuffer(iter->first, shader_stages);
			}
			else {
				uniformInputLayoutDescriptor->AddTexture(iter->first, shader_stages);
			}
		}
		uniformInputLayoutDescriptor->mDescriptorSetLayout = CreateDescriptorSetLayout(uniformInputLayoutDescriptor->mDescriptorSetLayoutBindings);
		uniformInputLayoutDescriptor->mDescriptorPool = CreateDescriptorPool(uniformInputLayoutDescriptor->mDescriptorPoolSize,1024);
		sUniformInputLayoutDescriptors.insert(std::pair<std::string, UniformInputLayoutDescriptor*>(uniformInputDescription->GetName(), uniformInputLayoutDescriptor));
	}
	void UniformInputLayoutDescriptor::AddUniformBuffer(int bindingPoint, VkShaderStageFlags shader_stage) {
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = bindingPoint;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding.stageFlags = shader_stage;
		mDescriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.descriptorCount = 1;
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		mDescriptorPoolSize.push_back(descriptorPoolSize);
	}
	void UniformInputLayoutDescriptor::AddTexture(int bindingPoint, VkShaderStageFlags shader_stage) {
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		descriptorSetLayoutBinding.binding = bindingPoint;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorSetLayoutBinding.descriptorCount = 1;
		descriptorSetLayoutBinding.stageFlags = shader_stage;
		descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
		mDescriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

		VkDescriptorPoolSize descriptorPoolSize = {};
		descriptorPoolSize.descriptorCount = 1;
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		mDescriptorPoolSize.push_back(descriptorPoolSize);
	}
}