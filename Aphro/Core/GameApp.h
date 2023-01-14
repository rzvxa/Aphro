#ifndef APH_GAME_APP_H
#define APH_GAME_APP_H

#include "GameObject.h"
#include "../Window/VulkanWindow.h";
#include "../Rendering/Vulkan/VulkanPipeline.h";
#include "../Rendering/Vulkan/VulkanDevice.h";
#include "../Rendering/Vulkan/VulkanSwapChain.h"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>

namespace aph {

	struct SimplePushConstantData {
		glm::mat2 transform{1.0f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	class GameApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		GameApp();
		~GameApp();

		GameApp(const GameApp&) = delete;
		GameApp& operator=(const GameApp&) = delete;

		void run();
	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		VulkanWindow m_window{ WIDTH, HEIGHT, "vkWindow" };
		VulkanDevice m_device{ m_window };
		std::unique_ptr<VulkanSwapChain> m_swapChain;
		std::unique_ptr<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<GameObject> m_gameObjects;
	};
}

#endif // !APH_GAME_APP_H