#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief NURBS / control-point spline. Speaks `rig.geometry.spline`.
 * @details Tessellation is fulfillment (`verticesOf`). Empty `weights` means
 * all ones (non-rational). `fitPoints` are optional authoring extras.
 */
struct CSpline {
	static constexpr bool kNoProperties = true;

	int degree = 3;
	bool closed = false;
	std::vector<glm::vec2> controlPoints;
	std::vector<float> knots;
	std::vector<float> weights;
	std::vector<glm::vec2> fitPoints;

	bool empty() const { return controlPoints.size() < 2 || knots.empty(); }
};

} // namespace ecs
} // namespace rigkit

