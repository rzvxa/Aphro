#ifndef APH_SIMPLE_RENDER_PIPELINE_H
#define APH_SIMPLE_RENDER_PIPELINE_H

#include "../Core/GameObject.h"
#include "../Core/Camera.h"
#include "Vulkan/VulkanPipeline.h";
#include "Vulkan/VulkanDevice.h";
#include "FrameInfo.h";


#include <memory>
#include <vector>

namespace aph {

	class SimpleRenderPipeline {
	public:
		SimpleRenderPipeline(VulkanDevice& device, VkRenderPass renderPass);
		~SimpleRenderPipeline();

		SimpleRenderPipeline(const SimpleRenderPipeline&) = delete;
		SimpleRenderPipeline& operator=(const SimpleRenderPipeline&) = delete;

		void renderGameObjects(
			FrameInfo& frameInfo,
			std::vector<GameObject>& gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice& m_device;

		std::unique_ptr<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}

#endif // !APH_SIMPLE_RENDER_PIPELINE_H