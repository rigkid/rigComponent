#pragma once

#include <algorithm>
#include <cmath>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Circular or elliptical arc between two angles. Speaks `rig.geometry.arc`.
 * @details Prefer `rx` / `ry`. Equal radii give a circle. Legacy documents may
 * only ship `radius`; hosts map that to both radii on import. Rotation of an
 * elliptical arc belongs on `CTransform`, not here.
 */
struct CArc {
	float cx = 0.0f;
	float cy = 0.0f;
	float rx = 50.0f;
	float ry = 50.0f;
	float startAngleDegrees = 0.0f;
	float endAngleDegrees = 90.0f;
	/// Close through the centre instead of chord to chord.
	bool pie = false;

	/** @brief Treat a legacy circular radius as both axes. */
	void setRadius(float r) {
		rx = r;
		ry = r;
	}

	/** @brief Larger axis — used for tessellation segment heuristics. */
	float majorRadius() const { return std::max(std::abs(rx), std::abs(ry)); }

	bool isCircular(float eps = 1e-4f) const { return std::abs(rx - ry) <= eps; }

	std::vector<sProp> GetProperties() {
		return {{0, "Center X", EPT_FLOAT, &cx},
				{1, "Center Y", EPT_FLOAT, &cy},
				{2, "Radius X", EPT_FLOAT, &rx},
				{3, "Radius Y", EPT_FLOAT, &ry},
				{4, "Start Angle", EPT_FLOAT, &startAngleDegrees},
				{5, "End Angle", EPT_FLOAT, &endAngleDegrees},
				{6, "Pie", EPT_BOOL, &pie}};
	}
};

} // namespace ecs
} // namespace rigkit

