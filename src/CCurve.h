#pragma once

#include "ecs/PropertyReflection.h"

#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace rigkit {
namespace ecs {

/**
 * @brief 1D transfer curve — speaks `rig.anim.curve`.
 * @details Remaps normalized t in [0,1] to [0,1]. Named presets regenerate
 * `points`; editing points sets `preset` to Custom. Runtime LUTs stay host-side.
 */
struct CCurve {
	enum class Interp : int { Linear = 0, Smooth = 1 };
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

	std::vector<glm::vec2> points{{0.f, 0.f}, {1.f, 1.f}};
	Interp interp = Interp::Smooth;
	Preset preset = Preset::Linear;

	static constexpr const char* kPresetNames[] = {"linear",  "ease-in", "ease-out", "ease-in-out",
												   "s-curve", "bulge",	 "squeeze",	 "custom"};
	static constexpr int kPresetCount = 8;
	static constexpr const char* kInterpNames[] = {"linear", "smooth"};
	static constexpr int kInterpCount = 2;

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
	return i == ecs::CCurve::Interp::Linear ? "linear" : "smooth";
}

inline ecs::CCurve::Interp interpFromId(const std::string& id) {
	return id == "linear" ? ecs::CCurve::Interp::Linear : ecs::CCurve::Interp::Smooth;
}

/** @brief Replace points from a named preset (Custom is a no-op). */
inline void applyPreset(ecs::CCurve& c, ecs::CCurve::Preset p) {
	c.preset = p;
	auto set = [&](std::initializer_list<glm::vec2> pts) { c.points.assign(pts); };
	switch (p) {
	case ecs::CCurve::Preset::Linear:
		set({{0.f, 0.f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::EaseIn:
		set({{0.f, 0.f}, {0.45f, 0.05f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::EaseOut:
		set({{0.f, 0.f}, {0.55f, 0.95f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::EaseInOut:
		set({{0.f, 0.f}, {0.35f, 0.05f}, {0.65f, 0.95f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::SCurve:
		set({{0.f, 0.f}, {0.25f, 0.1f}, {0.75f, 0.9f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::Bulge:
		set({{0.f, 0.f}, {0.25f, 0.12f}, {0.5f, 0.5f}, {0.75f, 0.88f}, {1.f, 1.f}});
		break;
	case ecs::CCurve::Preset::Squeeze:
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
	std::sort(c.points.begin(), c.points.end(),
			  [](const glm::vec2& a, const glm::vec2& b) { return a.x < b.x; });
	c.points.front().x = 0.f;
	c.points.back().x = 1.f;
	for (auto& p : c.points) {
		p.x = std::clamp(p.x, 0.f, 1.f);
		p.y = std::clamp(p.y, 0.f, 1.f);
	}
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
