#pragma once

#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Display name / stable id. Speaks `rig.meta.named`.
 * @details The document envelope writes this from `MEcs::entityName`. Keep the
 * two in sync with `setEntityName` — do not register a second `rig.meta.named`
 * codec (ProjectSerializer already emits the schema).
 */
struct CName {
	std::string name;
	std::string stableId;

	std::vector<sProp> GetProperties() {
		return {{0, "Name", EPT_STRING, &name}, {1, "Stable Id", EPT_STRING, &stableId}};
	}
};

} // namespace ecs
} // namespace rigkit
