#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Orbit framing params (yaw speed + spherical eye around `target`).
 * @details `SOrbitDrive` poses `CTransform` each Update when `enabled`.
 * Eye = target + (sin(yaw)*cos(pitch)*r, sin(pitch)*r, cos(yaw)*cos(pitch)*r).
 * `yaw` is a runtime cache (not reflected).
 */
struct COrbitDrive {
	float speed = 0.35f;
	float radius = 5.6f;
	float height = 2.1f; ///< Legacy cylindrical framing; prefer `pitch`
	float pitch = 0.36f; ///< Radians; clamp ~±1.4 in systems / nav
	bool enabled = true;
	glm::vec3 target{0.f, 0.5f, 0.f};
	float yaw = 0.f; ///< Runtime; not serialized / not in GetProperties

	std::vector<sProp> GetProperties() {
		return {{0, "Speed", EPT_FLOAT, &speed},
				{1, "Radius", EPT_FLOAT, &radius},
				{2, "Pitch", EPT_FLOAT, &pitch},
				{3, "Enabled", EPT_BOOL, &enabled},
				{4, "Target", EPT_VEC3, &target}};
	}
};

} // namespace ecs
} // namespace rigkit
