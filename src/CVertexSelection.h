#pragma once

#include <cstdint>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Selected vertex indices on a mesh entity.
 * @details Indices into `CMesh::positions`.
 */
struct CVertexSelection {
	std::vector<uint32_t> vertices;

	bool contains(uint32_t vertex) const {
		for (uint32_t v : vertices) {
			if (v == vertex) {
				return true;
			}
		}
		return false;
	}

	void clear() { vertices.clear(); }

	void toggle(uint32_t vertex) {
		for (auto it = vertices.begin(); it != vertices.end(); ++it) {
			if (*it == vertex) {
				vertices.erase(it);
				return;
			}
		}
		vertices.push_back(vertex);
	}

	int propCount = 0;

	std::vector<sProp> GetProperties() {
		propCount = static_cast<int>(vertices.size());
		return {{0, "Vertex count", EPT_INT, &propCount}};
	}
};

} // namespace ecs
} // namespace rigkit
