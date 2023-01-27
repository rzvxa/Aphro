#include "PointLightBillboard.h"

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace aph {

	struct PointLightPushConstants {
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLightBillboard::PointLightBillboard(
		VulkanDevice& device,
		VkRenderPass renderPass,
		VkDescriptorSetLayout globalSetLayout) : m_device(device) {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightBillboard::~PointLightBillboard() {
		vkDestroyPipelineLayout(m_device.device(), m_pipelineLayout, nullptr);
	}

	void PointLightBillboard::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

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

	void PointLightBillboard::createPipeline(VkRenderPass renderPass) {
		assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfigInfo{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfigInfo);
		pipelineConfigInfo.bindigDescriptions.clear();
		pipelineConfigInfo.attributeDescriptions.clear();
		pipelineConfigInfo.renderPass = renderPass;
		pipelineConfigInfo.pipelineLayout = m_pipelineLayout;
		m_pipeline = std::make_unique<VulkanPipeline>(
			m_device,
			"Assets/Aphro/Shaders/point_light.vert.spv",
			"Assets/Aphro/Shaders/point_light.frag.spv",
			pipelineConfigInfo);
	}

	void PointLightBillboard::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
		int lightIndex = 0;
		for (auto& kv : frameInfo.gameObjects) {
			auto& go = kv.second;
			if (go.pointLight == nullptr) continue;

			assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum number");

			ubo.pointLights[lightIndex].position = glm::vec4(go.transform.translation, 1.0f);
			ubo.pointLights[lightIndex].color = glm::vec4(go.color, go.pointLight->lightIntensity);

			lightIndex += 1;
		}

		ubo.numLights = lightIndex;
	}

	void PointLightBillboard::render(FrameInfo& frameInfo) {
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
			if (go.pointLight == nullptr) continue;

			PointLightPushConstants push{};
			push.position = glm::vec4(go.transform.translation, 1.f);
			push.color = glm::vec4(go.color, go.pointLight->lightIntensity);
			push.radius = go.transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				m_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);
			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}

	}
}