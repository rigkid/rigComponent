#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief NURBS / control-point spline in 3-space. Speaks `rig.geometry.spline3d`.
 * @details Tessellation is fulfillment. Empty `weights` means all ones.
 */
struct CSpline3d {
	static constexpr bool kNoProperties = true;

	int degree = 3;
	bool closed = false;
	std::vector<glm::vec3> controlPoints;
	std::vector<float> knots;
	std::vector<float> weights;
	std::vector<glm::vec3> fitPoints;

	bool empty() const { return controlPoints.size() < 2 || knots.empty(); }
};

} // namespace ecs
} // namespace rigkit
