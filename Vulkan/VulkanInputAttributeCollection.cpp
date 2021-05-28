#include "VulkanInputAttributeCollection.h"
#include "VulkanBufferObject.h"
namespace Alice {
	void VulkanInputAttributeCollection::Bind() {
		VulkanVertexBufferObject* vertexBuffer = (VulkanVertexBufferObject*)mVertexBuffers[0];
		ALICE_BIND_VERTEX_BUFFER(vertexBuffer->mBuffer);
	}
}