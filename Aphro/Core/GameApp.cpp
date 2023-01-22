#include "GameApp.h"

#include "Camera.h"
#include "../Rendering/SimpleRenderPipeline.h"

#include <stdexcept>
#include <chrono>
#include <array>

namespace aph {

	GameApp::GameApp() {
		loadGameObjects();
	}

	GameApp::~GameApp() {}

	void GameApp::run(std::function<void (GLFWwindow*, GameObject&, float)> update) {
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
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				renderPipeline.renderGameObjects(commandBuffer, m_gameObjects, camera);
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.device());
	}

    std::unique_ptr<Mesh> createCubeMesh(VulkanDevice& device, glm::vec3 offset) {
        std::vector<Mesh::Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<Mesh>(device, vertices);
    }

	void GameApp::loadGameObjects() {
        std::shared_ptr<Mesh> mesh = createCubeMesh(m_device, { .0f, .0f, .0f });
        auto cube = GameObject::createGameObject();
        cube.mesh = mesh;
        cube.transform.translation = { .0f, .0f, 2.5f };
        cube.transform.scale = { .5f, .5f, .5f };

        m_gameObjects.push_back(std::move(cube));
	}
}