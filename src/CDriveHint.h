#pragma once

#include <string>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Optional label: this entity is driven by a node-graph ref / out.
 * @details Plain string for Properties / Scene clarity (Pixile-style “linked”
 * cue). Bindings live on @c ref\.* nodes (`entity` + `prop`); this is display only.
 */
struct CDriveHint {
	std::string label;

	std::vector<sProp> GetProperties() {
		return {{0, "Graph drive", EPT_STRING, &label}};
	}
};

} // namespace ecs
} // namespace rigkit
