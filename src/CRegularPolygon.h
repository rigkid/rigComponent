#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Equilateral n-gon on a circle. Speaks `rig.geometry.regular_polygon`.
 * @details Stays parametric, so sides and radius remain editable. A triangle is
 * three sides. Arbitrary vertex lists belong on `CPolygon`.
 */
struct CRegularPolygon {
	float cx = 0.0f;
	float cy = 0.0f;
	/// Centre to vertex.
	float radius = 50.0f;
	int sides = 5;
	/// Shape-local spin from +X, independent of the entity transform.
	float rotationDegrees = 0.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Center X", EPT_FLOAT, &cx},
				{1, "Center Y", EPT_FLOAT, &cy},
				{2, "Radius", EPT_FLOAT, &radius},
				{3, "Sides", EPT_INT, &sides},
				{4, "Rotation", EPT_FLOAT, &rotationDegrees}};
	}
};

} // namespace ecs
} // namespace rigkit
