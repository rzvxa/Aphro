#include "GameApp.h"

#include "Camera.h"
#include "../Rendering/SimpleRenderPipeline.h"
#include "../Rendering/Vulkan/VulkanBuffer.h"

#include <stdexcept>
#include <chrono>
#include <array>

namespace aph {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	GameApp::GameApp() {
		loadGameObjects();
	}

	GameApp::~GameApp() {}

	void GameApp::run(std::function<void (GLFWwindow*, GameObject&, float)> update) {
		std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); ++i) {
			uboBuffers[i] = std::make_unique<VulkanBuffer>(
				m_device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		SimpleRenderPipeline renderPipeline{ m_device, m_renderer.getSwapChainRenderPass() };
        Camera camera{};
        //camera.setViewDirection(glm::vec3(.0f, .0f, -1.f), glm::vec3(.5f, .0f, 1.f));
        camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.f), glm::vec3(.0f, .0f, 2.5f));

        auto cameraObject = GameObject::createGameObject();

        auto nowTime = std::chrono::high_resolution_clock::now();

		while (!m_window.shouldClose()) {
			glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - nowTime).count();
            nowTime = newTime;

            update(m_window.getGLFWwindow(), cameraObject, deltaTime);
            camera.setViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);

            float aspectRatio = m_renderer.getAspectRatio();
            //camera.setOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);
            camera.setPrespectiveProjection(glm::radians(50.f), aspectRatio, .01f, 10.f);

			if (auto commandBuffer = m_renderer.beginFrame()) {
				int frameIndex = m_renderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					deltaTime,
					commandBuffer,
					camera,
				};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				renderPipeline.renderGameObjects(frameInfo, m_gameObjects);
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.device());
	}

	void GameApp::loadGameObjects() {
		std::shared_ptr<Mesh> mesh = Mesh::createMeshFromFile(m_device, "Assets/Aphro/Models/smooth_vase.obj");
        auto cube = GameObject::createGameObject();
        cube.mesh = mesh;
        cube.transform.translation = { .0f, .5f, 2.5f };
		cube.transform.scale = glm::vec3{ 3.f };

        m_gameObjects.push_back(std::move(cube));
	}
}