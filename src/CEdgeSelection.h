#pragma once

#include <cstdint>
#include <vector>

#include "ecs/PropertyReflection.h"
#include "MeshEdge.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Selected mesh edges on a mesh entity (runtime, like `CFaceSelection`).
 * @details Pairs into `CMesh::positions`, stored undirected (`min,max`). No Contract schema.
 */
struct CEdgeSelection {
	std::vector<MeshEdge> edges;

	bool contains(MeshEdge edge) const {
		edge = meshEdge(edge.a, edge.b);
		for (const MeshEdge& e : edges) {
			if (e == edge) {
				return true;
			}
		}
		return false;
	}

	void clear() { edges.clear(); }

	void toggle(MeshEdge edge) {
		edge = meshEdge(edge.a, edge.b);
		for (auto it = edges.begin(); it != edges.end(); ++it) {
			if (*it == edge) {
				edges.erase(it);
				return;
			}
		}
		edges.push_back(edge);
	}

	int propCount = 0;

	std::vector<sProp> GetProperties() {
		propCount = static_cast<int>(edges.size());
		return {{0, "Edge count", EPT_INT, &propCount}};
	}
};

} // namespace ecs
} // namespace rigkit
