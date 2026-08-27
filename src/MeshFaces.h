#pragma once

/**
 * @file
 * @brief Reads, welds, and tessellates the authored n-gon faces of a `CMesh`.
 * @details `loops` and `loopSizes` carry the authored faces; `indices` is the triangle list the
 * renderer draws. Call `meshFinishFaces` after create or import so the two stay in step.
 */

#include "CMesh.h"

#include <cstdint>

namespace rigkit {
namespace ecs {

/** @brief Authored face count (`loopSizes`, else triangle count). */
uint32_t meshFaceCount(const CMesh& mesh);

/**
 * @brief Range of one authored face in `loops`, or implicit triangle indices.
 * @return false when `face` is out of range.
 */
bool meshFaceRange(const CMesh& mesh, uint32_t face, uint32_t& begin, uint32_t& count);

/** @brief Vertex index `corner` of an authored face (0 .. count-1). */
uint32_t meshFaceCorner(const CMesh& mesh, uint32_t face, uint32_t corner);

/** @brief Newell normal of an authored face (unit length). */
glm::vec3 meshFaceNormal(const CMesh& mesh, uint32_t face);

/** @brief Fan-tessellate `loops` into `indices`. No-op when loops empty. */
void meshTriangulate(CMesh& mesh);

/**
 * @brief Map a tessellated triangle index to its authored face.
 * @details Identity when loops are empty.
 */
uint32_t meshTriangleToFace(const CMesh& mesh, uint32_t triangle);

/**
 * @brief If `loops` empty, each triangle becomes a 3-vert face (no coplanar weld).
 */
void meshEnsureLoops(CMesh& mesh);

/**
 * @brief Merge coincident positions, weld coplanar triangle islands into n-gons,
 *        then tessellate.
 * @details No-op for line modes. When loops already exist, only tessellates.
 * A cube's 12 triangles become 6 quads. Call after create / OBJ / classic import.
 */
void meshFinishFaces(CMesh& mesh);

/** @brief Authored edge count of one face (same as corner count). */
uint32_t meshFaceEdgeCount(const CMesh& mesh, uint32_t face);

/**
 * @brief Undirected vertex pair for one authored face edge (consecutive corners, plus close).
 * @return false when `face` / `edge` is out of range.
 */
bool meshFaceEdge(const CMesh& mesh, uint32_t face, uint32_t edge, uint32_t& a, uint32_t& b);

} // namespace ecs
} // namespace rigkit
