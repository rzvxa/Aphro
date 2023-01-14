#include "GameApp.h"

#include <stdexcept>
#include <array>

namespace aph {

	GameApp::GameApp() {
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	GameApp::~GameApp() {
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void GameApp::run() {
		while (!m_window.shouldClose()) {
			glfwPollEvents();
			drawFrame();
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

	void GameApp::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(m_device.device(), &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void GameApp::createPipeline() {
		assert(m_swapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfigInfo{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
		pipelineConfigInfo.renderPass = m_swapChain->getRenderPass();
		pipelineConfigInfo.pipelineLayout = m_pipelineLayout;
		m_pipeline = std::make_unique<VulkanPipeline>(
			m_device,
			"Assets/Aphro/Shaders/simple_shader.vert.spv",
			"Assets/Aphro/Shaders/simple_shader.frag.spv",
			pipelineConfigInfo);
	}

	void GameApp::recreateSwapChain() {
		auto extent = m_window.getExtend();
		while (extent.width == 0 || extent.height == 0) {
			extent = m_window.getExtend();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_device.device());
		if (m_swapChain == nullptr) {
			m_swapChain = std::make_unique<VulkanSwapChain>(m_device, extent);
		} else {
			m_swapChain = std::make_unique<VulkanSwapChain>(m_device, extent, std::move(m_swapChain));
			if (m_swapChain->imageCount() != m_commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}


		// TODO if render pass is compatible do not recreate pipeline
		createPipeline();
	}

	void GameApp::createCommandBuffers() {
		m_commandBuffers.resize(m_swapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

		if (vkAllocateCommandBuffers(m_device.device(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	}

	void GameApp::freeCommandBuffers() {
		vkFreeCommandBuffers(
			m_device.device(),
			m_device.getCommandPool(),
			static_cast<uint32_t>(m_commandBuffers.size()),
			m_commandBuffers.data());
		m_commandBuffers.clear();
	}

	void GameApp::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_swapChain->getRenderPass();
		renderPassInfo.framebuffer = m_swapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0},m_swapChain->getSwapChainExtent() };
		vkCmdSetViewport(m_commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(m_commandBuffers[imageIndex]);

		//vkCmdDraw(m_commandBuffers[imageIndex], 3, 1, 0, 0);

		vkCmdEndRenderPass(m_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}
	}

	void GameApp::renderGameObjects(VkCommandBuffer commandBuffer) {
		m_pipeline->bind(commandBuffer);

		for (auto& go : m_gameObjects) {
			SimplePushConstantData push{};
			push.offset = go.transform2d.translation;
			push.color = go.color;
			push.transform = go.transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			go.mesh->bind(commandBuffer);
			go.mesh->draw(commandBuffer);
		}
	}

	void GameApp::drawFrame() {
		uint32_t imageIndex;
		auto result = m_swapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = m_swapChain->submitCommandBuffers(&m_commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_window.wasWindowResized()) {
			m_window.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to acquire swap chain image");
		}
	}
}