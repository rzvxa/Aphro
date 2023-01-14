#ifndef APH_RENDERER_H
#define APH_RENDERER_H

#define APH_VULKAN_RENDERER

#ifdef APH_VULKAN_RENDERER
#include "../Window/VulkanWindow.h";
#include "Vulkan/VulkanDevice.h";
#include "Vulkan/VulkanSwapChain.h"
#else
#error "there is no specified render api"
#endif

#include <cassert>
#include <memory>
#include <vector>

namespace aph {

	class Renderer {
	public:
		Renderer(VulkanWindow& window, VulkanDevice& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
		bool isFrameInProgress() const { return m_isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
			return m_commandBuffers[m_currentImageIndex];
		}

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		VulkanWindow& m_window;
		VulkanDevice& m_device;
		std::unique_ptr<VulkanSwapChain> m_swapChain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_currentImageIndex;
		bool m_isFrameStarted{false};
	};
}

#endif // !APH_RENDERER_H
