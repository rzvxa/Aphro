#include <iostream>

#include "../Aphro/Core/GameApp.h"

#include "SimpleMovementController.h"


int main() {
	aph::GameApp app{};

	sandbox::SimpleMovementController movementController{};

	auto update = [&](GLFWwindow* window, aph::GameObject& gameObject, float dt) {
		movementController.moveInPlaneXZ(window, dt, gameObject);
	};

	try {
		app.run(update);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	std::cout << "GG." << std::endl;
	return EXIT_SUCCESS;
}