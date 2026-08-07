#pragma once

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Opt-in selection eligibility. Speaks `rig.interact.selectable`.
 * @details Selection state stays on `CSelection`. Geometry from transform + shape/mesh.
 * Absence = legacy pickable; when present, pick must honor `enabled`.
 */
struct CSelectable {
	bool enabled = true;

	std::vector<sProp> GetProperties() {
		return {{0, "Enabled", EPT_BOOL, &enabled}};
	}
};

} // namespace ecs
} // namespace rigkit
