#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Orbit framing params (yaw speed + spherical eye around `target`).
 * @details `SOrbitDrive` poses `CTransform` each Update when `enabled`.
 * Eye is spherical about `target` with yaw around `worldUp`. Default +Y is CAD
 * / `makeOrbitCamera`. Host Space (plotter bed on XY) uses +Z. `yaw` is a
 * runtime cache (not reflected).
 */
struct COrbitDrive {
	enum class WorldUp : int { Y = 0, Z = 1 };

	float speed = 0.35f;
	float radius = 5.6f;
	float height = 2.1f; ///< Legacy cylindrical framing; prefer `pitch`
	float pitch = 0.36f; ///< Radians; clamp ~±1.4 in systems / nav
	bool enabled = true;
	glm::vec3 target{0.f, 0.5f, 0.f};
	float yaw = 0.f; ///< Runtime; not serialized / not in GetProperties
	WorldUp worldUp = WorldUp::Y;

	/** @brief Unit world-up for lookAt / yaw axis. */
	glm::vec3 worldUpVec() const {
		return worldUp == WorldUp::Z ? glm::vec3{0.f, 0.f, 1.f} : glm::vec3{0.f, 1.f, 0.f};
	}

	std::vector<sProp> GetProperties() {
		static constexpr const char* kUpNames[] = {"Y", "Z"};
		return {{0, "Speed", EPT_FLOAT, &speed},
				{1, "Radius", EPT_FLOAT, &radius},
				{2, "Pitch", EPT_FLOAT, &pitch},
				{3, "Enabled", EPT_BOOL, &enabled},
				{4, "Target", EPT_VEC3, &target},
				{5, "World Up", EPT_ENUM, reinterpret_cast<int*>(&worldUp), kUpNames, 2}};
	}
};

} // namespace ecs
} // namespace rigkit
