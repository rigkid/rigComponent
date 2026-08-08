#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/** @brief Single segment with no interior; fill is ignored. Speaks `rig.geometry.line`. */
struct CLine {
	float x1 = 0.0f;
	float y1 = 0.0f;
	float x2 = 100.0f;
	float y2 = 100.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Start X", EPT_FLOAT, &x1},
				{1, "Start Y", EPT_FLOAT, &y1},
				{2, "End X", EPT_FLOAT, &x2},
				{3, "End Y", EPT_FLOAT, &y2}};
	}
};

} // namespace ecs
} // namespace rigkit
