#include "SimpleRenderPipeline.h"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace aph {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.0f };
		alignas(16) glm::vec3 color;
	};

	SimpleRenderPipeline::SimpleRenderPipeline(VulkanDevice& device, VkRenderPass renderPass) : m_device(device) {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderPipeline::~SimpleRenderPipeline() {
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void SimpleRenderPipeline::createPipelineLayout() {

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

	void SimpleRenderPipeline::createPipeline(VkRenderPass renderPass) {
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfigInfo{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
		pipelineConfigInfo.renderPass = renderPass;
		pipelineConfigInfo.pipelineLayout = m_pipelineLayout;
		m_pipeline = std::make_unique<VulkanPipeline>(
			m_device,
			"Assets/Aphro/Shaders/simple_shader.vert.spv",
			"Assets/Aphro/Shaders/simple_shader.frag.spv",
			pipelineConfigInfo);
	}

	void SimpleRenderPipeline::renderGameObjects(
		VkCommandBuffer commandBuffer,
		std::vector<GameObject>& gameObjects,
		const Camera& camera) {
		m_pipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& go : gameObjects) {
			go.transform.rotation.y = glm::mod(go.transform.rotation.y + 0.001f, glm::two_pi<float>());
			go.transform.rotation.x = glm::mod(go.transform.rotation.x + 0.0005f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.color = go.color;
			push.transform = projectionView * go.transform.mat4(); // TODO push this into shader

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
}