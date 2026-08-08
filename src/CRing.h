#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Annulus with a real hole. Speaks `rig.geometry.ring`.
 * @details A geometric hole, not a stroked ellipse. `innerRadius` never exceeds
 * `outerRadius`.
 */
struct CRing {
	float cx = 0.0f;
	float cy = 0.0f;
	float outerRadius = 50.0f;
	float innerRadius = 25.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Center X", EPT_FLOAT, &cx},
				{1, "Center Y", EPT_FLOAT, &cy},
				{2, "Outer Radius", EPT_FLOAT, &outerRadius},
				{3, "Inner Radius", EPT_FLOAT, &innerRadius}};
	}
};

} // namespace ecs
} // namespace rigkit
