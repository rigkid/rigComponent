#include "PathEllipticArc.h"

#include <algorithm>
#include <cmath>

namespace rigkit {
namespace path {
namespace {

constexpr float kPi = 3.14159265358979323846f;

float angleBetween(const glm::vec2& u, const glm::vec2& v) {
	const float n = std::sqrt(glm::dot(u, u) * glm::dot(v, v));
	if (n < 1e-12f) {
		return 0.f;
	}
	float c = glm::dot(u, v) / n;
	c = std::clamp(c, -1.f, 1.f);
	const float s = u.x * v.y - u.y * v.x;
	return (s < 0.f ? -1.f : 1.f) * std::acos(c);
}

struct CenterArc {
	glm::vec2 center{0.f, 0.f};
	float rx = 0.f;
	float ry = 0.f;
	float phi = 0.f;
	float theta1 = 0.f;
	float dTheta = 0.f;
	bool ok = false;
};

CenterArc toCenter(const EllipticArc& a) {
	CenterArc o;
	if (std::abs(a.start.x - a.end.x) < 1e-9f && std::abs(a.start.y - a.end.y) < 1e-9f) {
		return o;
	}
	float rx = std::abs(a.rx);
	float ry = std::abs(a.ry);
	if (rx < 1e-9f || ry < 1e-9f) {
		return o;
	}

	const float phi = a.xRotDeg * (kPi / 180.f);
	const float cosPhi = std::cos(phi);
	const float sinPhi = std::sin(phi);
	const glm::vec2 mid = (a.start - a.end) * 0.5f;
	const float x1p = cosPhi * mid.x + sinPhi * mid.y;
	const float y1p = -sinPhi * mid.x + cosPhi * mid.y;

	float rxSq = rx * rx;
	float rySq = ry * ry;
	const float x1pSq = x1p * x1p;
	const float y1pSq = y1p * y1p;
	const float lambda = x1pSq / rxSq + y1pSq / rySq;
	if (lambda > 1.f) {
		const float s = std::sqrt(lambda);
		rx *= s;
		ry *= s;
		rxSq = rx * rx;
		rySq = ry * ry;
	}

	const float num = rxSq * rySq - rxSq * y1pSq - rySq * x1pSq;
	const float den = rxSq * y1pSq + rySq * x1pSq;
	float cPrime = (den > 1e-12f) ? std::sqrt(std::max(0.f, num / den)) : 0.f;
	if (a.largeArc == a.sweep) {
		cPrime = -cPrime;
	}
	const float cxp = cPrime * (rx * y1p) / ry;
	const float cyp = cPrime * (-ry * x1p) / rx;
	o.center.x = cosPhi * cxp - sinPhi * cyp + (a.start.x + a.end.x) * 0.5f;
	o.center.y = sinPhi * cxp + cosPhi * cyp + (a.start.y + a.end.y) * 0.5f;
	o.rx = rx;
	o.ry = ry;
	o.phi = phi;

	const glm::vec2 v1{(x1p - cxp) / rx, (y1p - cyp) / ry};
	const glm::vec2 v2{(-x1p - cxp) / rx, (-y1p - cyp) / ry};
	o.theta1 = angleBetween({1.f, 0.f}, v1);
	o.dTheta = angleBetween(v1, v2);
	if (!a.sweep && o.dTheta > 0.f) {
		o.dTheta -= 2.f * kPi;
	} else if (a.sweep && o.dTheta < 0.f) {
		o.dTheta += 2.f * kPi;
	}
	o.ok = true;
	return o;
}

glm::vec2 atAngle(const CenterArc& c, float theta) {
	const float cosPhi = std::cos(c.phi);
	const float sinPhi = std::sin(c.phi);
	const float ct = std::cos(theta);
	const float st = std::sin(theta);
	return {c.center.x + c.rx * cosPhi * ct - c.ry * sinPhi * st,
			c.center.y + c.rx * sinPhi * ct + c.ry * cosPhi * st};
}

EllipticArc fromCenterSlice(const CenterArc& c, float theta0, float theta1, const glm::vec2& start,
							const glm::vec2& end) {
	EllipticArc a;
	a.start = start;
	a.end = end;
	a.rx = c.rx;
	a.ry = c.ry;
	a.xRotDeg = c.phi * (180.f / kPi);
	const float d = theta1 - theta0;
	a.largeArc = std::abs(d) > kPi;
	a.sweep = d > 0.f;
	return a;
}

} // namespace

ecs::CPath::Command makeArcTo(const glm::vec2& end, float rx, float ry, float xRotDeg,
							  bool largeArc, bool sweep) {
	ecs::CPath::Command cmd;
	cmd.type = ecs::CPath::Cmd::ArcTo;
	cmd.p = end;
	cmd.radii = {rx, ry};
	cmd.xRotDeg = xRotDeg;
	cmd.largeArc = largeArc;
	cmd.sweep = sweep;
	return cmd;
}

EllipticArc fromCommand(const glm::vec2& start, const ecs::CPath::Command& cmd) {
	EllipticArc a;
	a.start = start;
	a.end = cmd.p;
	a.rx = cmd.radii.x;
	a.ry = cmd.radii.y;
	a.xRotDeg = cmd.xRotDeg;
	a.largeArc = cmd.largeArc;
	a.sweep = cmd.sweep;
	return a;
}

void flatten(const EllipticArc& arc, float tol, std::vector<glm::vec2>& out) {
	const CenterArc c = toCenter(arc);
	if (!c.ok) {
		if (glm::length(arc.end - arc.start) > 1e-9f) {
			out.push_back(arc.end);
		}
		return;
	}
	const float r = std::max(c.rx, c.ry);
	const float useTol = tol > 0.f ? tol : 0.25f;
	int steps = 1;
	if (r > useTol) {
		const float maxStep = 2.f * std::acos(std::clamp(1.f - useTol / r, -1.f, 1.f));
		if (maxStep > 1e-6f) {
			steps = std::max(1, static_cast<int>(std::ceil(std::abs(c.dTheta) / maxStep)));
		}
	}
	for (int i = 1; i <= steps; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(steps);
		out.push_back(atAngle(c, c.theta1 + t * c.dTheta));
	}
	out.back() = arc.end;
}

void appendCubics(const EllipticArc& arc, ecs::CPath& out) {
	using Cmd = ecs::CPath::Cmd;
	const CenterArc c = toCenter(arc);
	if (!c.ok) {
		if (glm::length(arc.end - arc.start) > 1e-9f) {
			out.commands.push_back({Cmd::LineTo, arc.end, {}, {}});
		}
		return;
	}

	const int segments =
		std::max(1, static_cast<int>(std::ceil(std::abs(c.dTheta) / (kPi * 0.5f))));
	const float delta = c.dTheta / static_cast<float>(segments);
	const float t = std::tan(delta * 0.25f);
	const float alpha = std::sin(delta) * (std::sqrt(4.f + 3.f * t * t) - 1.f) / 3.f;
	const float cosPhi = std::cos(c.phi);
	const float sinPhi = std::sin(c.phi);

	float theta = c.theta1;
	for (int i = 0; i < segments; ++i) {
		const float theta2 = theta + delta;
		const float cos1 = std::cos(theta);
		const float sin1 = std::sin(theta);
		const float cos2 = std::cos(theta2);
		const float sin2 = std::sin(theta2);
		const glm::vec2 e0 = atAngle(c, theta);
		const glm::vec2 e1 = (i + 1 == segments) ? arc.end : atAngle(c, theta2);
		const glm::vec2 ep0{-c.rx * cosPhi * sin1 - c.ry * sinPhi * cos1,
							-c.rx * sinPhi * sin1 + c.ry * cosPhi * cos1};
		const glm::vec2 ep1{-c.rx * cosPhi * sin2 - c.ry * sinPhi * cos2,
							-c.rx * sinPhi * sin2 + c.ry * cosPhi * cos2};
		(void)e0;
		out.commands.push_back({Cmd::CubicTo, e1, e0 + alpha * ep0, e1 - alpha * ep1});
		theta = theta2;
	}
}

void applyLinear(ecs::CPath::Command& cmd, float a, float b, float c, float d) {
	if (cmd.type != ecs::CPath::Cmd::ArcTo) {
		return;
	}
	float rx = std::abs(cmd.radii.x);
	float ry = std::abs(cmd.radii.y);
	if (rx < 1e-12f || ry < 1e-12f) {
		return;
	}
	const float phi = cmd.xRotDeg * (kPi / 180.f);
	const float cosP = std::cos(phi);
	const float sinP = std::sin(phi);
	const float l00 = rx * cosP;
	const float l01 = -ry * sinP;
	const float l10 = rx * sinP;
	const float l11 = ry * cosP;
	const float n00 = a * l00 + c * l10;
	const float n01 = a * l01 + c * l11;
	const float n10 = b * l00 + d * l10;
	const float n11 = b * l01 + d * l11;

	const float m00 = n00 * n00 + n01 * n01;
	const float m01 = n00 * n10 + n01 * n11;
	const float m11 = n10 * n10 + n11 * n11;
	const float tr = m00 + m11;
	const float det = m00 * m11 - m01 * m01;
	const float disc = std::sqrt(std::max(0.f, tr * tr * 0.25f - det));
	const float ev1 = tr * 0.5f + disc;
	const float ev2 = tr * 0.5f - disc;
	cmd.radii.x = std::sqrt(std::max(0.f, ev1));
	cmd.radii.y = std::sqrt(std::max(0.f, ev2));

	if (std::abs(m01) > 1e-12f) {
		cmd.xRotDeg = std::atan2(m01, ev1 - m11) * (180.f / kPi);
	} else {
		// Axis-aligned result: atan2(0, ev1 - m11) is 0/0 when the major axis
		// lies on Y, so pick the axis by comparing the diagonal directly.
		cmd.xRotDeg = (m00 >= m11) ? 0.f : 90.f;
	}
	if (a * d - c * b < 0.f) {
		cmd.sweep = !cmd.sweep;
	}
}

bool split(const EllipticArc& arc, float t, EllipticArc& left, EllipticArc& right) {
	t = std::clamp(t, 0.05f, 0.95f);
	const CenterArc c = toCenter(arc);
	if (!c.ok) {
		return false;
	}
	const float midTh = c.theta1 + t * c.dTheta;
	const glm::vec2 mid = atAngle(c, midTh);
	left = fromCenterSlice(c, c.theta1, midTh, arc.start, mid);
	right = fromCenterSlice(c, midTh, c.theta1 + c.dTheta, mid, arc.end);
	return true;
}

} // namespace path
} // namespace rigkit
