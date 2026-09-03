#pragma once

#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Per-entity show/hide. Speaks `rig.render.visibility`.
 * @details Absent component means visible. Do not put `visible` on `CLayer`.
 */
struct CRenderVisibility {
	bool visible = true;

	std::vector<sProp> GetProperties() { return {{0, "Visible", EPT_BOOL, &visible}}; }
};

} // namespace ecs
} // namespace rigkit
