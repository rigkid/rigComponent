#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "CArc.h"
#include "CEllipse.h"
#include "CLine.h"
#include "CPolygon.h"
#include "CRectangle.h"
#include "CRegularPolygon.h"
#include "CRing.h"
#include "CStar.h"
#include "ecs/MEcs.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Axis-aligned extent of a primitive in its own local space.
 * @details Picking, selection handles, and framing want one answer regardless of
 * which primitive an entity carries. `valid` is false when there is nothing to
 * measure, such as a polygon with too few points.
 */
struct Bounds2D {
	glm::vec2 min{0.0f, 0.0f};
	glm::vec2 max{0.0f, 0.0f};
	bool valid = false;

	float width() const { return max.x - min.x; }
	float height() const { return max.y - min.y; }
	glm::vec2 center() const { return (min + max) * 0.5f; }
};

Bounds2D boundsOf(const CRectangle& shape);
Bounds2D boundsOf(const CEllipse& shape);
Bounds2D boundsOf(const CLine& shape);
Bounds2D boundsOf(const CPolygon& shape);
Bounds2D boundsOf(const CRegularPolygon& shape);
Bounds2D boundsOf(const CStar& shape);
Bounds2D boundsOf(const CArc& shape);
Bounds2D boundsOf(const CRing& shape);

/**
 * @brief Local bounds of whichever geometry primitive the entity carries.
 * @return Invalid bounds when the entity carries none.
 */
Bounds2D shapeBounds2D(MEcs& ecs, entt::entity entity);

/** @brief True when the entity carries any geometry primitive. */
bool hasShape2D(MEcs& ecs, entt::entity entity);

} // namespace ecs
} // namespace rigkit
