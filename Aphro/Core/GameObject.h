#ifndef APH_GAME_OBJECT_H
#define APH_GAME_OBJECT_H

#include "../Rendering/Mesh.h"

#include <memory>

namespace aph {

	struct Transform2dComponent {
		glm::vec2 translation{}; // position offset
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		glm::mat2 mat2() { 
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMat{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {scale.x, .0f}, {.0, scale.y} };
			return rotationMat * scaleMat;
		}
	};
	class GameObject {
	public:
		using id_t = unsigned int;

		static GameObject createGameObject() {
			static id_t currentId = 0;
			return GameObject{currentId++};
		}

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;


		std::shared_ptr<Mesh> mesh{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

	private:
		GameObject(id_t objId) : m_id(objId) {}


		id_t m_id;
	};
}

#endif // !APH_GAME_OBJECT_H
