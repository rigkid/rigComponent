#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Local TRS (+ euler editor cache) and cached world matrix.
 * @details Speaks `rig.spatial.transform` (`position` / `rotation` / `scale`).
 * `rotation` (quat) is authoritative for `localMatrix` and serialization — avoids
 * gimbal lock. `euler` is an editor-facing cache (radians); do not serialize it.
 * Use `setEulerRadians` / `setRotationQuat` (or the sync helpers) so both stay aligned.
 * `SHierarchy` fills `world` each Update from `CRelationship::parent` (if any).
 */
struct CTransform {
	glm::vec3 position{0.0f, 0.0f, 0.0f};
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale{1.0f, 1.0f, 1.0f};
	/// Editor-facing Euler (radians); not portable / not serialized. Keep in sync with `rotation`.
	glm::vec3 euler{0.0f, 0.0f, 0.0f};

	/// Filled by SHierarchy — parent chain * local TRS.
	glm::mat4 world{1.0f};

	void syncRotationFromEuler() { rotation = glm::normalize(glm::quat(euler)); }

	void syncEulerFromRotation() { euler = glm::eulerAngles(rotation); }

	void setEulerRadians(const glm::vec3& e) {
		euler = e;
		syncRotationFromEuler();
	}

	void setRotationQuat(const glm::quat& q) {
		rotation = glm::normalize(q);
		syncEulerFromRotation();
	}

	glm::mat4 localMatrix() const {
		return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) *
			   glm::scale(glm::mat4(1.0f), scale);
	}

	glm::vec2 worldTranslation2D() const { return {world[3][0], world[3][1]}; }

	glm::vec2 worldScale2D() const {
		return {glm::length(glm::vec2(world[0][0], world[0][1])),
				glm::length(glm::vec2(world[1][0], world[1][1]))};
	}

	std::vector<sProp> GetProperties() {
		return {{0, "Position X", EPT_FLOAT, &position.x},
				{1, "Position Y", EPT_FLOAT, &position.y},
				{2, "Position Z", EPT_FLOAT, &position.z},
				{3, "Rotation X (rad)", EPT_FLOAT, &euler.x},
				{4, "Rotation Y (rad)", EPT_FLOAT, &euler.y},
				{5, "Rotation Z (rad)", EPT_FLOAT, &euler.z},
				{6, "Scale X", EPT_FLOAT, &scale.x},
				{7, "Scale Y", EPT_FLOAT, &scale.y},
				{8, "Scale Z", EPT_FLOAT, &scale.z}};
	}
};

} // namespace ecs
} // namespace rigkit
