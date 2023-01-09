#ifndef APH_GAME_APP_H
#define APH_GAME_APP_H

#include "../Window/VulkanWindow.h";
#include "../Pipeline/VulkanPipeline.h";
#include "../Pipeline/VulkanDevice.h";
#include "../Pipeline/VulkanSwapChain.h"

#include <memory>
#include <vector>

namespace aph {
	class GameApp {
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		GameApp();
		~GameApp();

		GameApp(const GameApp&) = delete;
		GameApp& operator=(const GameApp&) = delete;

		void run();
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		VulkanWindow m_window{ WIDTH, HEIGHT, "vkWindow" };
		VulkanDevice m_device{ m_window };
		VulkanSwapChain m_swapChain{ m_device, m_window.getExtend() };
		std::unique_ptr<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
	};
}

#endif // !APH_GAME_APP_H