#include "VulkanWindow.h"
#include <iostream>

namespace aph {
	VulkanWindow::VulkanWindow(int w, int h, std::string name)
		: m_width(w), m_height(h), m_windowName(name) {
		init();
	}

	VulkanWindow::~VulkanWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VulkanWindow::init() {
		std::cout << "VulkanWindow Initialized." << std::endl;
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
	}

	void VulkanWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}
}