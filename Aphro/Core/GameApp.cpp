#include "GameApp.h"

#include "Camera.h"
#include "Rendering/Systems/SimpleRenderPipeline.h"
#include "Rendering/Systems/PointLightBillboard.h"
#include "Rendering/Vulkan/VulkanBuffer.h"

#include <stdexcept>
#include <chrono>
#include <array>

namespace aph {



	GameApp::GameApp() {
		m_globalPool =
			VulkanDescriptorPool::Builder(m_device)
			.setMaxSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	GameApp::~GameApp() {}

	void GameApp::run(std::function<void(GLFWwindow*, GameObject&, float)> update) {
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

		auto globalSetLayout = VulkanDescriptorSetLayout::Builder(m_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); ++i) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VulkanDescriptorWriter(*globalSetLayout, *m_globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderPipeline renderPipeline{
			m_device,
			m_renderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout()
		};

		PointLightBillboard pointLightBillboardSystem{
			m_device,
			m_renderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout()
		};


		Camera camera{};
		//camera.setViewDirection(glm::vec3(.0f, .0f, -1.f), glm::vec3(.5f, .0f, 1.f));
		camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.f), glm::vec3(.0f, .0f, 2.5f));

		auto cameraObject = GameObject::createGameObject();
		cameraObject.transform.translation.z = -2.5f;

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
					globalDescriptorSets[frameIndex],
					m_gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				pointLightBillboardSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				renderPipeline.renderGameObjects(frameInfo);
				pointLightBillboardSystem.render(frameInfo);
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.device());
	}

	void GameApp::loadGameObjects() {
		std::shared_ptr<Mesh> mesh = Mesh::createMeshFromFile(m_device, "Assets/Aphro/Models/smooth_vase.obj");
		auto v1 = GameObject::createGameObject();
		v1.mesh = mesh;
		v1.transform.translation = { -.5f, .5f, 0.f };
		v1.transform.scale = glm::vec3{ 3.f };
		m_gameObjects.emplace(v1.getId(), std::move(v1));

		mesh = Mesh::createMeshFromFile(m_device, "Assets/Aphro/Models/smooth_vase.obj");
		auto v2 = GameObject::createGameObject();
		v2.mesh = mesh;
		v2.transform.translation = { .5f, .5f, 0.f };
		v2.transform.scale = glm::vec3{ 3.f };
		m_gameObjects.emplace(v2.getId(), std::move(v2));

		mesh = Mesh::createMeshFromFile(m_device, "Assets/Aphro/Models/quad.obj");
		auto floor = GameObject::createGameObject();
		floor.mesh = mesh;
		floor.transform.translation = { .0f, .5f, 0.f };
		floor.transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		m_gameObjects.emplace(floor.getId(), std::move(floor));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};
		for (int i = 0; i < lightColors.size(); ++i) {
			auto pointLight = GameObject::makePointLight(.4f);
			pointLight.color = lightColors[i];
			auto rotate = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f }
			);
			pointLight.transform.translation = glm::vec3(rotate * glm::vec4(-1.f, -1.f, -1.f , 1.f));
			m_gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
}