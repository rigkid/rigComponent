#include "PrimitiveVertices.h"

#include <algorithm>
#include <cmath>

namespace rigkit {
namespace ecs {
namespace {

constexpr float kTwoPi = 6.28318530717958647692f;
constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;

glm::vec2 onCircle(float cx, float cy, float radius, float angleRad) {
	return {cx + std::cos(angleRad) * radius, cy + std::sin(angleRad) * radius};
}

std::vector<glm::vec2> ring(float cx, float cy, float outer, float inner, int count,
							float rotationDegrees, bool alternate) {
	std::vector<glm::vec2> points;
	if (count < 3) {
		return points;
	}
	const int total = alternate ? count * 2 : count;
	points.reserve(static_cast<std::size_t>(total));
	const float step = kTwoPi / static_cast<float>(total);
	const float base = rotationDegrees * kDegToRad;
	for (int i = 0; i < total; ++i) {
		const float radius = (alternate && (i % 2 == 1)) ? inner : outer;
		points.push_back(onCircle(cx, cy, radius, base + step * static_cast<float>(i)));
	}
	return points;
}

} // namespace

int segmentsForRadius(float radius, int requested) {
	if (requested > 0) {
		return std::max(requested, 3);
	}
	const int scaled = static_cast<int>(std::abs(radius) * 0.5f);
	return std::clamp(scaled, 12, 128);
}

std::vector<glm::vec2> verticesOf(const CRegularPolygon& shape) {
	return ring(shape.cx, shape.cy, shape.radius, shape.radius, shape.sides,
				shape.rotationDegrees, false);
}

std::vector<glm::vec2> verticesOf(const CStar& shape) {
	return ring(shape.cx, shape.cy, shape.radius, shape.innerRadius, shape.points,
				shape.rotationDegrees, true);
}

std::vector<glm::vec2> verticesOf(const CArc& shape, int segments) {
	std::vector<glm::vec2> points;
	const float startRad = shape.startAngleDegrees * kDegToRad;
	const float endRad = shape.endAngleDegrees * kDegToRad;
	const float sweep = endRad - startRad;
	// Sweep, not radius alone, decides the step — a 5 degree arc needs few points.
	const int full = segmentsForRadius(shape.radius, segments);
	const int count =
		std::max(2, static_cast<int>(std::ceil(std::abs(sweep) / kTwoPi * static_cast<float>(full))));
	points.reserve(static_cast<std::size_t>(count) + 2);
	for (int i = 0; i <= count; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(count);
		points.push_back(onCircle(shape.cx, shape.cy, shape.radius, startRad + sweep * t));
	}
	if (shape.pie) {
		points.push_back({shape.cx, shape.cy});
	}
	return points;
}

void rimsOf(const CRing& shape, std::vector<glm::vec2>& outer, std::vector<glm::vec2>& inner,
			int segments) {
	const int count = segmentsForRadius(shape.outerRadius, segments);
	outer.clear();
	inner.clear();
	outer.reserve(static_cast<std::size_t>(count));
	inner.reserve(static_cast<std::size_t>(count));
	const float step = kTwoPi / static_cast<float>(count);
	for (int i = 0; i < count; ++i) {
		const float angle = step * static_cast<float>(i);
		outer.push_back(onCircle(shape.cx, shape.cy, shape.outerRadius, angle));
		inner.push_back(onCircle(shape.cx, shape.cy, shape.innerRadius, angle));
	}
}

} // namespace ecs
} // namespace rigkit
