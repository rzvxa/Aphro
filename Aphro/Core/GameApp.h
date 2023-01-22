#ifndef APH_GAME_APP_H
#define APH_GAME_APP_H

#include "GameObject.h"
#include "../Window/VulkanWindow.h";
#include "../Rendering/Vulkan/VulkanDevice.h";
#include "../Rendering/Renderer.h"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>
#include <vector>
#include <functional>

namespace aph {

	class GameApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		GameApp();
		~GameApp();

		GameApp(const GameApp&) = delete;
		GameApp& operator=(const GameApp&) = delete;

		void run(std::function<void (GLFWwindow*, GameObject&, float)> update);
	private:
		void loadGameObjects();

		VulkanWindow m_window{ WIDTH, HEIGHT, "vkWindow" };
		VulkanDevice m_device{ m_window };
		Renderer m_renderer{ m_window, m_device };

		std::vector<GameObject> m_gameObjects;
	};
}

#endif // !APH_GAME_APP_H