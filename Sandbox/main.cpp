#include <iostream>
#include "../Aphro/Core/GameApp.h"

int main() {
	aph::GameApp app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	std::cout << "GG." << std::endl;
	return EXIT_SUCCESS;
}