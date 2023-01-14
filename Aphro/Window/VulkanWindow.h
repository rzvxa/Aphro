#ifndef APH_VULKAN_WINDOW_H
#define APH_VULKAN_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace aph {
	class VulkanWindow {
	public:
		VulkanWindow(int w, int h, std::string name);
		~VulkanWindow();

		VulkanWindow(const VulkanWindow&) = delete;
		VulkanWindow& operator=(const VulkanWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); };
		VkExtent2D getExtend() { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; };
		bool wasWindowResized() { return m_frameBufferResized; }
		void resetWindowResizedFlag() { m_frameBufferResized = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void init();

		int m_width;
		int m_height;
		bool m_frameBufferResized = false;

		std::string m_windowName;
		GLFWwindow* window;
	};
}

#endif // !APH_VULKAN_WINDOW_H