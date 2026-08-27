#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Icosphere-tessellated sphere. Speaks `rig.geometry.sphere`.
 * @details Stays parametric like the 2D primitives (`CRegularPolygon`, `CStar`, ...):
 * radius and subdivisions remain editable, and present rebuilds triangles fresh
 * each frame (`verticesOf(const CSphere&)` in `PrimitiveVertices3D.h`) rather than
 * baking a one-shot `CMesh`. Reach for `CMesh` directly instead when a shape needs
 * per-face colors or a one-off hand-authored form.
 */
struct CSphere {
	float radius = 1.0f;
	/// Icosahedron refinement steps: 0 = base 20 faces, each step is 4x more faces.
	int subdivisions = 1;

	std::vector<sProp> GetProperties() {
		return {{0, "Radius", EPT_FLOAT, &radius}, {1, "Subdivisions", EPT_INT, &subdivisions}};
	}
};

} // namespace ecs
} // namespace rigkit
