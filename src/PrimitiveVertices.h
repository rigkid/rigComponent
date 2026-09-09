#pragma once

/**
 * @file
 * @brief Expands the parametric 2D primitives into local-space vertex lists.
 * @details Segment counts follow radius, so small shapes stay cheap to tessellate on Raspberry Pi.
 */

#include <glm/glm.hpp>
#include <vector>

#include "CArc.h"
#include "CRegularPolygon.h"
#include "CRing.h"
#include "CSpline.h"
#include "CStar.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Vertex expansion for the parametric primitives.
 * @details Drawing, picking, and bounds all need the same points. Deriving them
 * in one place is what keeps a star the same star everywhere.
 */

/// Curve tessellation for @p radius, clamped so tiny shapes stay cheap and big
/// ones stay smooth. Pass a positive @p requested to override.
int segmentsForRadius(float radius, int requested = 0);

/** @brief Corner vertices of a regular n-gon, rotation included. Empty below three sides. */
std::vector<glm::vec2> verticesOf(const CRegularPolygon& shape);

/** @brief Alternating outer and inner vertices of a star. Empty below three points. */
std::vector<glm::vec2> verticesOf(const CStar& shape);

/**
 * @brief Arc as a polyline from start angle to end angle.
 * @details Includes the centre as the final point when `pie` is set, so the
 * result closes into a wedge. Uses `rx`/`ry` (elliptical when unequal).
 */
std::vector<glm::vec2> verticesOf(const CArc& shape, int segments = 0);

/**
 * @brief Spline samples along the knot domain (closed loops repeat the first point).
 */
std::vector<glm::vec2> verticesOf(const CSpline& shape, int segments = 0);

/**
 * @brief Point at normalized parameter @p t in [0, 1] along the knot domain.
 * @return {0,0} when the spline cannot be evaluated.
 */
glm::vec2 pointOn(const CSpline& shape, float t);

/** @brief Outer and inner rims of a ring, same winding and matching lengths. */
void rimsOf(const CRing& shape, std::vector<glm::vec2>& outer, std::vector<glm::vec2>& inner,
			int segments = 0);

} // namespace ecs
} // namespace rigkit

