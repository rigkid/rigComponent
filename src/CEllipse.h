#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/** @brief Ellipse from centre and radii; equal radii give a circle. Speaks `rig.geometry.ellipse`. */
struct CEllipse {
	float cx = 0.0f;
	float cy = 0.0f;
	float rx = 50.0f;
	float ry = 50.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Center X", EPT_FLOAT, &cx},
				{1, "Center Y", EPT_FLOAT, &cy},
				{2, "Radius X", EPT_FLOAT, &rx},
				{3, "Radius Y", EPT_FLOAT, &ry}};
	}
};

} // namespace ecs
} // namespace rigkit
