#pragma once
#include "../InputAttributeCollection.h"
namespace Alice {
	class VulkanInputAttributeCollection :public InputAttributeCollection {
	public:
		void Bind();
	};
}