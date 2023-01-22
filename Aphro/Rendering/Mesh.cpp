#include "Mesh.h"

namespace aph {

	Mesh::Mesh(VulkanDevice& device, const Mesh::Builder& builder) : m_device(device) {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	Mesh::~Mesh() {
		vkDestroyBuffer(m_device.device(), m_vertexBuffer, nullptr);
		vkFreeMemory(m_device.device(), m_vertexBufferMemory, nullptr);

		if (m_hasIndexBuffer) {
			vkDestroyBuffer(m_device.device(), m_indexBuffer, nullptr);
			vkFreeMemory(m_device.device(), m_indexBufferMemory, nullptr);
		}
	}

	void Mesh::createVertexBuffers(const std::vector<Vertex>& vertecies) {
		m_vertexCount = static_cast<uint32_t>(vertecies.size());
		assert(m_vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertecies[0]) * m_vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory staginBufferMemory;
		m_device.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			staginBufferMemory);

		void* data;
		vkMapMemory(m_device.device(), staginBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertecies.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_device.device(), staginBufferMemory);

		m_device.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_vertexBuffer,
			m_vertexBufferMemory);

		m_device.copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

		vkDestroyBuffer(m_device.device(), stagingBuffer, nullptr);
		vkFreeMemory(m_device.device(), staginBufferMemory, nullptr);
	}

	void Mesh::createIndexBuffers(const std::vector<uint32_t>& indices) {
		m_indexCount = static_cast<uint32_t>(indices.size());
		m_hasIndexBuffer = m_indexCount > 0;

		if (!m_hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory staginBufferMemory;
		m_device.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			staginBufferMemory);

		void* data;
		vkMapMemory(m_device.device(), staginBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_device.device(), staginBufferMemory);

		m_device.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_indexBuffer,
			m_indexBufferMemory);

		m_device.copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

		vkDestroyBuffer(m_device.device(), stagingBuffer, nullptr);
		vkFreeMemory(m_device.device(), staginBufferMemory, nullptr);
	}

	void Mesh::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (m_hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Mesh::draw(VkCommandBuffer commandBuffer) {
		if (m_hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
		}
	}

	std::vector<VkVertexInputBindingDescription> Mesh::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Mesh::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
}