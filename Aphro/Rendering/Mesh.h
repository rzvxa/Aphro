#ifndef APH_MESH_H
#define APH_MESH_H

#include "../Rendering/Vulkan/VulkanDevice.h"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace aph {
	class Mesh {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		Mesh(VulkanDevice& device, const Builder& vertecies);
		~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertecies);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		VulkanDevice& m_device;

		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;
		
		bool m_hasIndexBuffer = false;
		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;
		uint32_t m_indexCount;
	};
}

#endif // !APH_Mesh_H
