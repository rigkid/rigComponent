#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Star from outer and inner radii. Speaks `rig.geometry.star`.
 * @details `innerRadius` is an absolute distance in the same units as `radius`,
 * not a fraction of it.
 */
struct CStar {
	float cx = 0.0f;
	float cy = 0.0f;
	/// Centre to outer point.
	float radius = 50.0f;
	/// Centre to inner vertex, absolute.
	float innerRadius = 25.0f;
	/// Outer point count.
	int points = 5;
	/// Shape-local spin from +X, independent of the entity transform.
	float rotationDegrees = 0.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Center X", EPT_FLOAT, &cx},
				{1, "Center Y", EPT_FLOAT, &cy},
				{2, "Radius", EPT_FLOAT, &radius},
				{3, "Inner Radius", EPT_FLOAT, &innerRadius},
				{4, "Points", EPT_INT, &points},
				{5, "Rotation", EPT_FLOAT, &rotationDegrees}};
	}
};

} // namespace ecs
} // namespace rigkit
