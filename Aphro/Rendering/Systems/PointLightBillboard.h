#ifndef APH_POINT_LIGHT_BILLBOARD_H
#define APH_POINT_LIGHT_BILLBOARD_H

#include "Core/GameObject.h"
#include "Core/Camera.h"
#include "Rendering/Vulkan/VulkanPipeline.h";
#include "Rendering/Vulkan/VulkanDevice.h";
#include "Rendering/FrameInfo.h";


#include <memory>
#include <vector>

namespace aph {

	class PointLightBillboard {
	public:
		PointLightBillboard(VulkanDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightBillboard();

		PointLightBillboard(const PointLightBillboard&) = delete;
		PointLightBillboard& operator=(const PointLightBillboard&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice& m_device;

		std::unique_ptr<VulkanPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}

#endif // !APH_POINT_LIGHT_BILLBOARD_H