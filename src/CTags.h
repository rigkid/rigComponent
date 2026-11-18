#pragma once

/**
 * @file
 * @brief Free-form labels. Speaks `rig.meta.tags`.
 */

#include <algorithm>
#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Host-authored labels for selection and filtering.
 * @details Contract does not read tag strings. Hosts only honour tags they
 * authored (e.g. Relay POD export omits layers tagged `guide` / `proof`).
 */
struct CTags {
	std::vector<std::string> tags;

	std::vector<sProp> GetProperties() { return {}; }

	bool has(const std::string& tag) const {
		return std::find(tags.begin(), tags.end(), tag) != tags.end();
	}

	void add(const std::string& tag) {
		if (tag.empty() || has(tag)) {
			return;
		}
		tags.push_back(tag);
	}
};

} // namespace ecs
} // namespace rigkit
