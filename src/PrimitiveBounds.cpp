#include "PrimitiveBounds.h"

#include <algorithm>
#include <cmath>

#include "PrimitiveVertices.h"

namespace rigkit {
namespace ecs {

namespace {

Bounds2D fromCorners(glm::vec2 a, glm::vec2 b) {
	Bounds2D out;
	out.min = {std::min(a.x, b.x), std::min(a.y, b.y)};
	out.max = {std::max(a.x, b.x), std::max(a.y, b.y)};
	out.valid = true;
	return out;
}

Bounds2D fromPoints(const glm::vec2* points, std::size_t count) {
	Bounds2D out;
	if (count == 0) {
		return out;
	}
	out.min = points[0];
	out.max = points[0];
	for (std::size_t i = 1; i < count; ++i) {
		out.min.x = std::min(out.min.x, points[i].x);
		out.min.y = std::min(out.min.y, points[i].y);
		out.max.x = std::max(out.max.x, points[i].x);
		out.max.y = std::max(out.max.y, points[i].y);
	}
	out.valid = true;
	return out;
}

} // namespace

Bounds2D boundsOf(const CRectangle& shape) {
	return fromCorners({shape.x, shape.y}, {shape.x + shape.width, shape.y + shape.height});
}

Bounds2D boundsOf(const CEllipse& shape) {
	return fromCorners({shape.cx - shape.rx, shape.cy - shape.ry},
					   {shape.cx + shape.rx, shape.cy + shape.ry});
}

Bounds2D boundsOf(const CLine& shape) {
	return fromCorners({shape.x1, shape.y1}, {shape.x2, shape.y2});
}

Bounds2D boundsOf(const CPolygon& shape) {
	if (shape.points.size() < 3) {
		return {};
	}
	return fromPoints(shape.points.data(), shape.points.size());
}

Bounds2D boundsOf(const CRegularPolygon& shape) {
	const auto points = verticesOf(shape);
	return fromPoints(points.data(), points.size());
}

Bounds2D boundsOf(const CStar& shape) {
	const auto points = verticesOf(shape);
	return fromPoints(points.data(), points.size());
}

Bounds2D boundsOf(const CArc& shape) {
	// Measuring the drawn polyline keeps bounds honest when the sweep crosses an
	// axis, where the two endpoints alone understate the extent.
	const auto points = verticesOf(shape);
	return fromPoints(points.data(), points.size());
}

Bounds2D boundsOf(const CRing& shape) {
	return fromCorners({shape.cx - shape.outerRadius, shape.cy - shape.outerRadius},
					   {shape.cx + shape.outerRadius, shape.cy + shape.outerRadius});
}

Bounds2D shapeBounds2D(MEcs& ecs, entt::entity entity) {
	if (ecs.hasComponent<CRectangle>(entity)) {
		return boundsOf(ecs.getComponent<CRectangle>(entity));
	}
	if (ecs.hasComponent<CEllipse>(entity)) {
		return boundsOf(ecs.getComponent<CEllipse>(entity));
	}
	if (ecs.hasComponent<CLine>(entity)) {
		return boundsOf(ecs.getComponent<CLine>(entity));
	}
	if (ecs.hasComponent<CPolygon>(entity)) {
		return boundsOf(ecs.getComponent<CPolygon>(entity));
	}
	if (ecs.hasComponent<CRegularPolygon>(entity)) {
		return boundsOf(ecs.getComponent<CRegularPolygon>(entity));
	}
	if (ecs.hasComponent<CStar>(entity)) {
		return boundsOf(ecs.getComponent<CStar>(entity));
	}
	if (ecs.hasComponent<CArc>(entity)) {
		return boundsOf(ecs.getComponent<CArc>(entity));
	}
	if (ecs.hasComponent<CRing>(entity)) {
		return boundsOf(ecs.getComponent<CRing>(entity));
	}
	return {};
}

bool hasShape2D(MEcs& ecs, entt::entity entity) {
	return ecs.hasComponent<CRectangle>(entity) || ecs.hasComponent<CEllipse>(entity) ||
		   ecs.hasComponent<CLine>(entity) || ecs.hasComponent<CPolygon>(entity) ||
		   ecs.hasComponent<CRegularPolygon>(entity) || ecs.hasComponent<CStar>(entity) ||
		   ecs.hasComponent<CArc>(entity) || ecs.hasComponent<CRing>(entity);
}

} // namespace ecs
} // namespace rigkit
