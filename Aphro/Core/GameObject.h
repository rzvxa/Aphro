#ifndef APH_GAME_OBJECT_H
#define APH_GAME_OBJECT_H

#include "../Rendering/Mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace aph {

	struct TransformComponent {
		glm::vec3 translation{}; // position offset
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation;

		glm::mat4 mat4() {
			auto transform = glm::translate(glm::mat4{ 1.f }, translation);

			// tait-bryan angles
			transform = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
			transform = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
			transform = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });

			transform = glm::scale(transform, scale);
			return transform;
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
		TransformComponent transform{};

	private:
		GameObject(id_t objId) : m_id(objId) {}


		id_t m_id;
	};
}

#endif // !APH_GAME_OBJECT_H
