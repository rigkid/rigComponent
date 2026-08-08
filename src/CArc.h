#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Circular arc between two angles. Speaks `rig.geometry.arc`.
 * @details Angles are degrees from +X turning toward +Y, and may wrap past 360.
 * Elliptical arcs belong on `CPath`.
 */
struct CArc {
	float cx = 0.0f;
	float cy = 0.0f;
	float radius = 50.0f;
	float startAngleDegrees = 0.0f;
	float endAngleDegrees = 90.0f;
	/// Close through the centre instead of chord to chord.
	bool pie = false;

	std::vector<sProp> GetProperties() {
		return {{0, "Center X", EPT_FLOAT, &cx},
				{1, "Center Y", EPT_FLOAT, &cy},
				{2, "Radius", EPT_FLOAT, &radius},
				{3, "Start Angle", EPT_FLOAT, &startAngleDegrees},
				{4, "End Angle", EPT_FLOAT, &endAngleDegrees},
				{5, "Pie", EPT_BOOL, &pie}};
	}
};

} // namespace ecs
} // namespace rigkit
