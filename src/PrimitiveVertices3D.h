#pragma once

/**
 * @file
 * @brief Expands the parametric 3D primitives into local-space triangle vertices.
 * @details Subdivision is clamped, since face count grows as 20 * 4^n and Raspberry Pi pays for
 * every triangle.
 */

#include <glm/glm.hpp>
#include <vector>

#include "CSphere.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Vertex expansion for the parametric 3D primitives.
 * @details Mirrors `PrimitiveVertices.h` (2D): present, picking, and bounds all
 * want the same points, so deriving them once here is what keeps a sphere the
 * same sphere everywhere.
 */

/**
 * @brief Loose (non-indexed) triangle soup for an icosphere, local space.
 * @details `subdivisions` clamped to 0..4 so a stray large value cannot blow the
 * triangle count up (20 * 4^n faces); flat per-face shading does not need shared
 * vertices, so faces are emitted independently.
 */
std::vector<glm::vec3> verticesOf(const CSphere& shape);

} // namespace ecs
} // namespace rigkit
