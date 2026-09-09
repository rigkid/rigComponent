#include "PrimitiveVertices.h"

#include <algorithm>
#include <cmath>
#include "CSpline.h"

namespace rigkit {
namespace ecs {
namespace {

constexpr float kTwoPi = 6.28318530717958647692f;
constexpr float kDegToRad = 3.14159265358979323846f / 180.0f;

glm::vec2 onCircle(float cx, float cy, float radius, float angleRad) {
	return {cx + std::cos(angleRad) * radius, cy + std::sin(angleRad) * radius};
}

glm::vec2 onEllipse(float cx, float cy, float rx, float ry, float angleRad) {
	return {cx + std::cos(angleRad) * rx, cy + std::sin(angleRad) * ry};
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

/** @brief de Boor on homogeneous control points `(x*w, y*w, w)`. */
glm::vec3 deBoor(int degree, int span, float u, const std::vector<float>& knots,
				 const std::vector<glm::vec3>& ctrl) {
	std::vector<glm::vec3> d(static_cast<std::size_t>(degree) + 1);
	for (int j = 0; j <= degree; ++j) {
		const int idx = span - degree + j;
		d[static_cast<std::size_t>(j)] = (idx >= 0 && idx < static_cast<int>(ctrl.size()))
											 ? ctrl[static_cast<std::size_t>(idx)]
											 : glm::vec3(0.f);
	}
	for (int r = 1; r <= degree; ++r) {
		for (int j = degree; j >= r; --j) {
			const int i = span - degree + j;
			const float denom = knots[static_cast<std::size_t>(i + degree - r + 1)] -
								knots[static_cast<std::size_t>(i)];
			const float alpha =
				(denom <= 0.f) ? 0.f
							   : (u - knots[static_cast<std::size_t>(i)]) / denom;
			d[static_cast<std::size_t>(j)] =
				(1.f - alpha) * d[static_cast<std::size_t>(j - 1)] +
				alpha * d[static_cast<std::size_t>(j)];
		}
	}
	return d[static_cast<std::size_t>(degree)];
}

glm::vec2 projectHomogeneous(const glm::vec3& h) {
	if (std::abs(h.z) <= 1e-12f) {
		return {h.x, h.y};
	}
	return {h.x / h.z, h.y / h.z};
}

bool splineDomain(const CSpline& shape, int& n, float& u0, float& u1,
				  std::vector<glm::vec3>& ctrl) {
	if (shape.empty() || shape.degree < 1) {
		return false;
	}
	n = static_cast<int>(shape.controlPoints.size()) - 1;
	if (n < shape.degree) {
		return false;
	}
	if (static_cast<int>(shape.knots.size()) < n + shape.degree + 2) {
		return false;
	}
	u0 = shape.knots[static_cast<std::size_t>(shape.degree)];
	u1 = shape.knots[static_cast<std::size_t>(n + 1)];
	if (!(u1 > u0)) {
		return false;
	}
	ctrl.resize(shape.controlPoints.size());
	const bool rational =
		!shape.weights.empty() && shape.weights.size() == shape.controlPoints.size();
	for (std::size_t i = 0; i < shape.controlPoints.size(); ++i) {
		const float w = rational ? shape.weights[i] : 1.f;
		ctrl[i] = {shape.controlPoints[i].x * w, shape.controlPoints[i].y * w, w};
	}
	return true;
}

int findSpan(int n, int degree, float u, const std::vector<float>& knots) {
	if (u >= knots[static_cast<std::size_t>(n + 1)]) {
		return n;
	}
	if (u <= knots[static_cast<std::size_t>(degree)]) {
		return degree;
	}
	int low = degree;
	int high = n + 1;
	int mid = (low + high) / 2;
	while (u < knots[static_cast<std::size_t>(mid)] ||
		   u >= knots[static_cast<std::size_t>(mid + 1)]) {
		if (u < knots[static_cast<std::size_t>(mid)]) {
			high = mid;
		} else {
			low = mid;
		}
		mid = (low + high) / 2;
	}
	return mid;
}

glm::vec2 evalSpline(const CSpline& shape, float u, int n, const std::vector<glm::vec3>& ctrl) {
	const int span = findSpan(n, shape.degree, u, shape.knots);
	return projectHomogeneous(deBoor(shape.degree, span, u, shape.knots, ctrl));
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
	const int full = segmentsForRadius(shape.majorRadius(), segments);
	const int count =
		std::max(2, static_cast<int>(std::ceil(std::abs(sweep) / kTwoPi * static_cast<float>(full))));
	points.reserve(static_cast<std::size_t>(count) + 2);
	for (int i = 0; i <= count; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(count);
		points.push_back(onEllipse(shape.cx, shape.cy, shape.rx, shape.ry, startRad + sweep * t));
	}
	if (shape.pie) {
		points.push_back({shape.cx, shape.cy});
	}
	return points;
}

std::vector<glm::vec2> verticesOf(const CSpline& shape, int segments) {
	std::vector<glm::vec2> points;
	int n = 0;
	float u0 = 0.f;
	float u1 = 0.f;
	std::vector<glm::vec3> ctrl;
	if (!splineDomain(shape, n, u0, u1, ctrl)) {
		return points;
	}

	float lengthHint = 0.f;
	for (size_t i = 1; i < shape.controlPoints.size(); ++i) {
		lengthHint += glm::length(shape.controlPoints[i] - shape.controlPoints[i - 1]);
	}
	const int count = segments > 0
						  ? std::max(segments, 2)
						  : std::clamp(static_cast<int>(lengthHint * 0.5f), 16, 256);
	points.reserve(static_cast<std::size_t>(count) + 1);
	for (int i = 0; i <= count; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(count);
		points.push_back(evalSpline(shape, u0 + (u1 - u0) * t, n, ctrl));
	}
	if (shape.closed && !points.empty()) {
		points.push_back(points.front());
	}
	return points;
}

glm::vec2 pointOn(const CSpline& shape, float t) {
	int n = 0;
	float u0 = 0.f;
	float u1 = 0.f;
	std::vector<glm::vec3> ctrl;
	if (!splineDomain(shape, n, u0, u1, ctrl)) {
		return {};
	}
	t = std::clamp(t, 0.f, 1.f);
	return evalSpline(shape, u0 + (u1 - u0) * t, n, ctrl);
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
