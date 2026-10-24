#pragma once

/**
 * @file
 * @brief `CCurve` transfer-curve component with its preset, evaluation, and LUT helpers.
 * @details Data and pure functions only; the point-dragging widget lives in `rigVectorEditor`.
 */

#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief 1D transfer curve - speaks `rig.anim.curve`.
 * @details Remaps normalized t in [0,1] to [0,1]. Named presets regenerate
 * `points` and set interpolation: linear is a straight line, the others are
 * smooth. Editing points sets `preset` to Custom. Runtime LUTs stay host-side.
 * `handles` are relative in/out offsets (cubic Bezier) when `interp` is Bezier.
 */
struct CCurve {
	enum class Interp : int { Linear = 0, Smooth = 1, Bezier = 2 };
	enum class Preset : int {
		Linear = 0,
		EaseIn,
		EaseOut,
		EaseInOut,
		SCurve,
		Bulge,
		Squeeze,
		Custom
	};

	struct Handle {
		glm::vec2 in{0.f};
		glm::vec2 out{0.f};
	};

	std::vector<glm::vec2> points{{0.f, 0.f}, {1.f, 1.f}};
	std::vector<Handle> handles{{}, {}};
	Interp interp = Interp::Linear;
	Preset preset = Preset::Linear;

	static constexpr const char* kPresetNames[] = {"linear",  "ease-in", "ease-out", "ease-in-out",
												   "s-curve", "bulge",	 "squeeze",	 "custom"};
	static constexpr int kPresetCount = 8;
	static constexpr const char* kInterpNames[] = {"linear", "smooth", "bezier"};
	static constexpr int kInterpCount = 3;

	std::vector<sProp> GetProperties() {
		return {
			{0, "Preset", EPT_ENUM, reinterpret_cast<int*>(&preset), kPresetNames, kPresetCount},
			{1, "Interp", EPT_ENUM, reinterpret_cast<int*>(&interp), kInterpNames, kInterpCount}};
	}
};

} // namespace ecs

namespace curve {

inline const char* presetId(ecs::CCurve::Preset p) {
	const int i = static_cast<int>(p);
	if (i < 0 || i >= ecs::CCurve::kPresetCount) {
		return "custom";
	}
	return ecs::CCurve::kPresetNames[i];
}

inline ecs::CCurve::Preset presetFromId(const std::string& id) {
	for (int i = 0; i < ecs::CCurve::kPresetCount; ++i) {
		if (id == ecs::CCurve::kPresetNames[i]) {
			return static_cast<ecs::CCurve::Preset>(i);
		}
	}
	return ecs::CCurve::Preset::Custom;
}

inline const char* interpId(ecs::CCurve::Interp i) {
	switch (i) {
	case ecs::CCurve::Interp::Linear:
		return "linear";
	case ecs::CCurve::Interp::Bezier:
		return "bezier";
	default:
		return "smooth";
	}
}

inline ecs::CCurve::Interp interpFromId(const std::string& id) {
	if (id == "linear") {
		return ecs::CCurve::Interp::Linear;
	}
	if (id == "bezier") {
		return ecs::CCurve::Interp::Bezier;
	}
	return ecs::CCurve::Interp::Smooth;
}

/**
 * @brief Replace points from a named preset (Custom is a no-op).
 * @details Linear uses linear interpolation, so the diagonal is a straight line.
 * The other presets use smooth interpolation. Handles are cleared.
 */
inline void applyPreset(ecs::CCurve& c, ecs::CCurve::Preset p) {
	c.preset = p;
	auto set = [&](std::initializer_list<glm::vec2> pts) {
		c.points.assign(pts);
		c.handles.assign(c.points.size(), {});
	};
	switch (p) {
	case ecs::CCurve::Preset::Linear:
		c.interp = ecs::CCurve::Interp::Linear;
		set({{0.f, 0.f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::EaseIn:
		c.interp = ecs::CCurve::Interp::Smooth;
		set({{0.f, 0.f}, {0.45f, 0.05f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::EaseOut:
		c.interp = ecs::CCurve::Interp::Smooth;
		set({{0.f, 0.f}, {0.55f, 0.95f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::EaseInOut:
		c.interp = ecs::CCurve::Interp::Smooth;
		set({{0.f, 0.f}, {0.35f, 0.05f}, {0.65f, 0.95f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::SCurve:
		c.interp = ecs::CCurve::Interp::Smooth;
		set({{0.f, 0.f}, {0.25f, 0.1f}, {0.75f, 0.9f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::Bulge:
		c.interp = ecs::CCurve::Interp::Smooth;
		set({{0.f, 0.f}, {0.25f, 0.12f}, {0.5f, 0.5f}, {0.75f, 0.88f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::Squeeze:
		c.interp = ecs::CCurve::Interp::Smooth;
		set({{0.f, 0.f}, {0.25f, 0.35f}, {0.5f, 0.5f}, {0.75f, 0.65f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::Custom:
		break;
	}
}

inline void ensureValid(ecs::CCurve& c) {
	if (c.points.size() < 2) {
		applyPreset(c, ecs::CCurve::Preset::Linear);
	}
	if (c.handles.size() != c.points.size()) {
		c.handles.resize(c.points.size());
	}
	const size_t n = c.points.size();
	std::vector<size_t> idx(n);
	for (size_t i = 0; i < n; ++i) {
		idx[i] = i;
	}
	std::sort(idx.begin(), idx.end(),
			  [&](size_t a, size_t b) { return c.points[a].x < c.points[b].x; });
	std::vector<glm::vec2> pts(n);
	std::vector<ecs::CCurve::Handle> hs(n);
	for (size_t i = 0; i < n; ++i) {
		pts[i] = c.points[idx[i]];
		hs[i] = c.handles[idx[i]];
	}
	c.points = std::move(pts);
	c.handles = std::move(hs);
	c.points.front().x = 0.f;
	c.points.back().x = 1.f;
	for (auto& p : c.points) {
		p.x = std::clamp(p.x, 0.f, 1.f);
		p.y = std::clamp(p.y, 0.f, 1.f);
	}
	for (size_t i = 0; i + 1 < n; ++i) {
		const float dx = std::max(0.f, c.points[i + 1].x - c.points[i].x);
		c.handles[i].out.x = std::clamp(c.handles[i].out.x, 0.f, dx);
		c.handles[i + 1].in.x = std::clamp(c.handles[i + 1].in.x, -dx, 0.f);
	}
	c.handles.front().in = {0.f, 0.f};
	c.handles.back().out = {0.f, 0.f};
}

/** @brief Convert the polyline to cubic Bezier with Catmull-Rom tangents. */
inline void toBezier(ecs::CCurve& c) {
	ensureValid(c);
	c.interp = ecs::CCurve::Interp::Bezier;
	c.handles.assign(c.points.size(), {});
	const auto& pts = c.points;
	const size_t n = pts.size();
	auto slopeAt = [&](size_t i) -> float {
		if (n < 2) {
			return 0.f;
		}
		if (i == 0) {
			const float dx = std::max(1e-6f, pts[1].x - pts[0].x);
			return (pts[1].y - pts[0].y) / dx;
		}
		if (i + 1 == n) {
			const float dx = std::max(1e-6f, pts[n - 1].x - pts[n - 2].x);
			return (pts[n - 1].y - pts[n - 2].y) / dx;
		}
		const float dx = std::max(1e-6f, pts[i + 1].x - pts[i - 1].x);
		return (pts[i + 1].y - pts[i - 1].y) / dx;
	};
	for (size_t i = 0; i < n; ++i) {
		const float sl = slopeAt(i);
		if (i > 0) {
			const float dx = pts[i].x - pts[i - 1].x;
			c.handles[i].in.x = -dx / 3.f;
			c.handles[i].in.y = sl * c.handles[i].in.x;
		}
		if (i + 1 < n) {
			const float dx = pts[i + 1].x - pts[i].x;
			c.handles[i].out.x = dx / 3.f;
			c.handles[i].out.y = sl * c.handles[i].out.x;
		}
	}
	ensureValid(c);
}

inline float bezierComp(float a, float c1, float c2, float b, float s) {
	const float is = 1.f - s;
	return is * is * is * a + 3.f * is * is * s * c1 + 3.f * is * s * s * c2 + s * s * s * b;
}

inline float bezierCompD(float a, float c1, float c2, float b, float s) {
	const float is = 1.f - s;
	return 3.f * is * is * (c1 - a) + 6.f * is * s * (c2 - c1) + 3.f * s * s * (b - c2);
}

/** @brief Solve cubic Bezier x(s) = t for s in [0,1]. Newton with bisection clamp. */
inline float solveBezierS(float ax, float c1x, float c2x, float bx, float t) {
	float lo = 0.f;
	float hi = 1.f;
	float s = (bx - ax > 1e-6f) ? (t - ax) / (bx - ax) : 0.f;
	s = std::clamp(s, 0.f, 1.f);
	for (int i = 0; i < 8; ++i) {
		const float x = bezierComp(ax, c1x, c2x, bx, s);
		if (x < t) {
			lo = s;
		} else {
			hi = s;
		}
		const float dx = bezierCompD(ax, c1x, c2x, bx, s);
		if (std::fabs(dx) > 1e-6f) {
			s -= (x - t) / dx;
		} else {
			s = 0.5f * (lo + hi);
		}
		s = std::clamp(s, lo, hi);
		if (std::fabs(x - t) < 1e-6f) {
			break;
		}
	}
	return s;
}

inline float eval(const ecs::CCurve& c, float t) {
	t = std::clamp(t, 0.f, 1.f);
	if (c.points.size() < 2) {
		return t;
	}
	const auto& pts = c.points;
	if (t <= pts.front().x) {
		return pts.front().y;
	}
	if (t >= pts.back().x) {
		return pts.back().y;
	}
	size_t i = 0;
	while (i + 1 < pts.size() && pts[i + 1].x < t) {
		++i;
	}
	const glm::vec2& a = pts[i];
	const glm::vec2& b = pts[std::min(i + 1, pts.size() - 1)];
	const float dx = std::max(1e-6f, b.x - a.x);
	const float u = (t - a.x) / dx;
	if (c.interp == ecs::CCurve::Interp::Bezier && c.handles.size() == pts.size()) {
		const size_t j = std::min(i + 1, pts.size() - 1);
		const glm::vec2 c1 = a + c.handles[i].out;
		const glm::vec2 c2 = b + c.handles[j].in;
		const float s = solveBezierS(a.x, c1.x, c2.x, b.x, t);
		return std::clamp(bezierComp(a.y, c1.y, c2.y, b.y, s), 0.f, 1.f);
	}
	float s = u;
	if (c.interp == ecs::CCurve::Interp::Smooth) {
		s = u * u * (3.f - 2.f * u);
	}
	return std::clamp(a.y + (b.y - a.y) * s, 0.f, 1.f);
}

inline void buildLut(const ecs::CCurve& c, std::vector<float>& out, int n = 256) {
	n = std::max(2, n);
	out.resize(static_cast<size_t>(n));
	for (int i = 0; i < n; ++i) {
		out[static_cast<size_t>(i)] = eval(c, i / static_cast<float>(n - 1));
	}
}

inline float sampleLut(const std::vector<float>& lut, float t) {
	if (lut.empty()) {
		return std::clamp(t, 0.f, 1.f);
	}
	t = std::clamp(t, 0.f, 1.f);
	const float f = t * static_cast<float>(lut.size() - 1);
	const int i0 = static_cast<int>(f);
	const int i1 = std::min(i0 + 1, static_cast<int>(lut.size()) - 1);
	const float u = f - static_cast<float>(i0);
	return lut[static_cast<size_t>(i0)] * (1.f - u) + lut[static_cast<size_t>(i1)] * u;
}

} // namespace curve
} // namespace rigkit
