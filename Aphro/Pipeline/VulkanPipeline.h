#ifndef APH_VULKAN_PIPELINE_H
#define APH_VULKAN_PIPELINE_H

#include "VulkanDevice.h"
#include "../Graphics/Model.h"

#include <string>
#include <vector>

namespace aph {

	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo& other) {
			viewport = other.viewport;
			scissor = other.scissor;
			inputAssemblyInfo = other.inputAssemblyInfo;
			rasterizationInfo = other.rasterizationInfo;
			multisampleInfo = other.multisampleInfo;
			colorBlendAttachment = other.colorBlendAttachment;
			colorBlendInfo = other.colorBlendInfo;
			colorBlendInfo.pAttachments = &colorBlendAttachment;
			depthStencilInfo = other.depthStencilInfo;
			pipelineLayout = other.pipelineLayout;
			renderPass = other.renderPass;
		}
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};


	class VulkanPipeline {
	public:
		VulkanPipeline(
			VulkanDevice& device,
			const std::string& vert_path,
			const std::string& frag_path,
			const PipelineConfigInfo& configInfo);

		~VulkanPipeline();

		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			const std::string& vert_file,
			const std::string& frag_file,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		VulkanDevice& m_device;
		VkPipeline m_graphicsPipeline;
		VkShaderModule m_vertShaderModule;
		VkShaderModule m_fragShaderModule;
	};
}

#endif // !APH_VULKAN_PIPELINE_H
