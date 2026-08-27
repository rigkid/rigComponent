#pragma once

/**
 * @file
 * @brief Builds, flattens, splits, and transforms SVG elliptical arcs held as `CPath::ArcTo`.
 * @details The portable `.rig` path schema has no arc command, so expand to cubics before saving.
 */

#include <glm/glm.hpp>
#include <vector>
#include "CPath.h"

namespace rigkit {
namespace path {

/**
 * @brief SVG F.6 elliptical arc kept as CPath::ArcTo.
 * @details Flatten / fill / CNC tessellate through these helpers. Portable
 * `.rig` path schema has no arc command — expand with @ref appendCubics.
 */

struct EllipticArc {
	glm::vec2 start{0.f, 0.f};
	glm::vec2 end{0.f, 0.f};
	float rx = 0.f;
	float ry = 0.f;
	float xRotDeg = 0.f;
	bool largeArc = false;
	bool sweep = false;
};

/** @brief Build an ArcTo command (endpoint + SVG A parameters). */
ecs::CPath::Command makeArcTo(const glm::vec2& end, float rx, float ry, float xRotDeg,
							  bool largeArc, bool sweep);

EllipticArc fromCommand(const glm::vec2& start, const ecs::CPath::Command& cmd);

/** @brief Sample the arc (endpoint last). Empty when the arc collapses to a point. */
void flatten(const EllipticArc& arc, float tol, std::vector<glm::vec2>& out);

/** @brief Append CubicTo approximations (SVG F.6). LineTo when a radius is zero. */
void appendCubics(const EllipticArc& arc, ecs::CPath& out);

/**
 * @brief Map ellipse axes through a 2x2 (SVG matrix a,b,c,d). Flips sweep if det < 0.
 * @details Endpoints are transformed by the caller. Radii / rotation update here.
 */
void applyLinear(ecs::CPath::Command& cmd, float a, float b, float c, float d);

/** @brief Split at t in (0,1) along the swept angle. Both pieces stay ArcTo. */
bool split(const EllipticArc& arc, float t, EllipticArc& left, EllipticArc& right);

} // namespace path
} // namespace rigkit
