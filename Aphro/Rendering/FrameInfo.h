#ifndef APH_FRAME_INFO_H
#define APH_FRAME_INFO_H

#include "../Core/Camera.h"
#include "../Core/GameObject.h"

typedef VkCommandBuffer;

namespace aph {

#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int index;
		float time;
		VkCommandBuffer commandBuffer; // abstract this
		Camera& camera;
		VkDescriptorSet globalDescriptorSet;
		GameObject::Map& gameObjects;
	};
}

#endif // !APH_FRAME_INFO_H
