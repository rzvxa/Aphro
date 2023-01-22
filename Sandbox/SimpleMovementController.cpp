#include "SimpleMovementController.h"

#include <limits>
#include <iostream>

namespace sandbox {
	void SimpleMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, aph::GameObject& gameObject) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		glm::vec3 rotate{ 0 };
		if (glfwGetMouseButton(window, keys.orbitMouseButton) == GLFW_PRESS) {
			auto mousePosition = glm::vec2(xpos, ypos);
			auto mouseDelta = m_lastMousePosition - mousePosition;
			m_lastMousePosition = mousePosition;
			if (!glm::all(glm::lessThan(glm::abs(mouseDelta), glm::vec2(std::numeric_limits<float>::epsilon())))) {

				std::cout << "mouse pos (" << xpos << ", " << ypos << ")" << std::endl;
				std::cout << "mouse delta (" << mouseDelta.x << ", " << mouseDelta.y << ")" << std::endl;
				if (mouseDelta.x < 0) rotate.y += 1.f;
				if (mouseDelta.x > 0) rotate.y -= 1.f;
				if (mouseDelta.y < 0) rotate.x -= 1.f;
				if (mouseDelta.y > 0) rotate.x += 1.f;
			}
		}


		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
		}
	}
}