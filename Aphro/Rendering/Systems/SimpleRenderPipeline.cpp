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
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderPipeline::SimpleRenderPipeline(
		VulkanDevice& device,
		VkRenderPass renderPass,
		VkDescriptorSetLayout globalSetLayout) : m_device(device) {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	SimpleRenderPipeline::~SimpleRenderPipeline() {
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void SimpleRenderPipeline::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
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

	void SimpleRenderPipeline::renderGameObjects(FrameInfo& frameInfo) {
		m_pipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0, 1,
			&frameInfo.globalDescriptorSet,
			0, nullptr
		);

		for (auto& kv : frameInfo.gameObjects) {
			auto& go = kv.second;
			if (go.mesh == nullptr) continue;
			SimplePushConstantData push{};
			push.modelMatrix = go.transform.mat4();
			push.normalMatrix = go.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			go.mesh->bind(frameInfo.commandBuffer);
			go.mesh->draw(frameInfo.commandBuffer);
		}
	}
}