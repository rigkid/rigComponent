#pragma once

/**
 * @file
 * @brief Vertex-pair key naming one undirected edge of a mesh.
 * @details Derived from face loops when needed; the mesh keeps no edge table of its own.
 */

#include <cstdint>

namespace rigkit {
namespace ecs {

/**
 * @brief Undirected mesh edge as a vertex pair into `CMesh::positions`.
 * @details Store `min(a,b)`, `max(a,b)`. Speaks the `{a,b}` objects on
 * `rig.cad.fillet` / `rig.cad.chamfer`. Derived from consecutive `loops` pairs —
 * not a parallel edge table on the mesh.
 */
struct MeshEdge {
	uint32_t a = 0;
	uint32_t b = 0;
};

inline MeshEdge meshEdge(uint32_t i, uint32_t j) {
	MeshEdge e;
	e.a = i < j ? i : j;
	e.b = i < j ? j : i;
	return e;
}

inline bool operator==(const MeshEdge& x, const MeshEdge& y) {
	return x.a == y.a && x.b == y.b;
}

} // namespace ecs
} // namespace rigkit
