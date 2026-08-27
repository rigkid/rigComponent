#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief NURBS surface (control net). Speaks `rig.geometry.nurbs_surface`.
 * @details `controlPoints` is row-major, U varies fastest, length `countU * countV`.
 * Tessellation to `CMesh` is fulfillment.
 */
struct CNurbsSurface {
	static constexpr bool kNoProperties = true;

	int degreeU = 3;
	int degreeV = 3;
	int countU = 0;
	int countV = 0;
	std::vector<glm::vec3> controlPoints;
	std::vector<float> knotsU;
	std::vector<float> knotsV;
	std::vector<float> weights;
	bool closedU = false;
	bool closedV = false;

	bool empty() const {
		return countU < 2 || countV < 2 ||
			   controlPoints.size() != static_cast<size_t>(countU) * static_cast<size_t>(countV) ||
			   knotsU.empty() || knotsV.empty();
	}
};

} // namespace ecs
} // namespace rigkit
