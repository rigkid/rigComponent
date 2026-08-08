#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Freeform outline as a resolved vertex list. Speaks `rig.geometry.polygon`.
 * @details Needs at least three points. Parametric n-gons stay on
 * `CRegularPolygon`, which survives a change of side count.
 */
struct CPolygon {
	std::vector<glm::vec2> points;
	bool closed = true;

	bool empty() const { return points.size() < 3; }

	/// Vertices are bulk data, so only the flag is editor-visible.
	std::vector<sProp> GetProperties() {
		return {{0, "Closed", EPT_BOOL, &closed}};
	}
};

} // namespace ecs
} // namespace rigkit
