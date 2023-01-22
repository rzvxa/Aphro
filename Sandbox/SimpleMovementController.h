#ifndef APH_SANDBOX_FPS_MOVEMENT_CONTROLLER_H
#define APH_SANDBOX_FPS_MOVEMENT_CONTROLLER_H

#include "../Aphro/Core/GameObject.h"
#include "../Aphro/Window/VulkanWindow.h"
#include <iostream>
namespace sandbox {
	class SimpleMovementController {
	public:

        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, aph::GameObject& gameObject);

        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.5f };
	};
}

#endif // !APH_SANDBOX_FPS_MOVEMENT_CONTROLLER_H
