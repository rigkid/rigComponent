#pragma once

#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Orbit framing params (yaw speed + eye radius/height).
 * @details App or node `ref.*` sinks write these; Update reads them to pose the camera.
 * Speaks portable orbit-drive fields — no window / graph pointers.
 */
struct COrbitDrive {
	float speed = 0.35f;
	float radius = 5.6f;
	float height = 2.1f;

	std::vector<sProp> GetProperties() {
		return {{0, "Speed", EPT_FLOAT, &speed},
				{1, "Radius", EPT_FLOAT, &radius},
				{2, "Height", EPT_FLOAT, &height}};
	}
};

} // namespace ecs
} // namespace rigkit
