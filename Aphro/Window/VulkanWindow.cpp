#include "VulkanWindow.h"
#include <iostream>

namespace aph {
	VulkanWindow::VulkanWindow(int w, int h, std::string name)
		: m_width(w), m_height(h), m_windowName(name) {
		init();
	}

	VulkanWindow::~VulkanWindow() {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void VulkanWindow::init() {
		std::cout << "VulkanWindow Initialized." << std::endl;
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, frameBufferResizeCallback);
	}

	void VulkanWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

	void VulkanWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vulkanWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

		vulkanWindow->m_frameBufferResized = true;
		vulkanWindow->m_width = width;
		vulkanWindow->m_height = height;
	}
}