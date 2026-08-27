#pragma once

#include <cstdint>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Selected triangle face indices on a mesh entity.
 * @details Indices into authored faces (`CMesh::loopSizes`, else triangles).
 */
struct CFaceSelection {
	std::vector<uint32_t> faces;

	bool contains(uint32_t face) const {
		for (uint32_t f : faces) {
			if (f == face) {
				return true;
			}
		}
		return false;
	}

	void clear() { faces.clear(); }

	void toggle(uint32_t face) {
		for (auto it = faces.begin(); it != faces.end(); ++it) {
			if (*it == face) {
				faces.erase(it);
				return;
			}
		}
		faces.push_back(face);
	}

	int propCount = 0;

	std::vector<sProp> GetProperties() {
		propCount = static_cast<int>(faces.size());
		return {{0, "Face count", EPT_INT, &propCount}};
	}
};

} // namespace ecs
} // namespace rigkit
