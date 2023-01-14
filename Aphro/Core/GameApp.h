#ifndef APH_GAME_APP_H
#define APH_GAME_APP_H

#include "../Window/VulkanWindow.h";
#include "../Pipeline/VulkanPipeline.h";
#include "../Pipeline/VulkanDevice.h";
#include "../Pipeline/VulkanSwapChain.h"
#include "../Graphics/Model.h"

#include <memory>
#include <vector>

namespace aph {
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
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		VulkanWindow m_window{ WIDTH, HEIGHT, "vkWindow" };
		VulkanDevice m_device{ m_window };
		std::unique_ptr<VulkanSwapChain> m_swapChain;
		std::unique_ptr<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::unique_ptr<Model> m_model;
	};
}

#endif // !APH_GAME_APP_H