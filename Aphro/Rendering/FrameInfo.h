#ifndef APH_FRAME_INFO_H
#define APH_FRAME_INFO_H

#include "../Core/Camera.h"
#include "../Core/GameObject.h"

typedef VkCommandBuffer;

namespace aph {
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
