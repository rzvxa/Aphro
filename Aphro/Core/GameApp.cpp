#include "GameApp.h"

#include "../Rendering/SimpleRenderPipeline.h"

#include <stdexcept>
#include <array>

namespace aph {

	GameApp::GameApp() {
		loadGameObjects();
	}

	GameApp::~GameApp() {}

	void GameApp::run() {
		SimpleRenderPipeline renderPipeline{ m_device, m_renderer.getSwapChainRenderPass() };

		while (!m_window.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = m_renderer.beginFrame()) {
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				renderPipeline.renderGameObjects(commandBuffer, m_gameObjects);
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.device());
	}

	void GameApp::loadGameObjects() {
		std::vector<Mesh::Vertex> vertecies{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		auto mesh = std::make_shared<Mesh>(m_device, vertecies);
		auto go = GameObject::createGameObject();
		go.mesh = mesh;
		go.color = { .1f, .8f, .1f };
		go.transform2d.translation.x = .2f;
		go.transform2d.scale = { 2.f, .5f };
		go.transform2d.rotation = .25f * glm::two_pi<float>();

		m_gameObjects.push_back(std::move(go));
	}
}