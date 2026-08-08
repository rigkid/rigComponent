#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/** @brief Rectangle anchored at its top-left corner. Speaks `rig.geometry.rectangle`. */
struct CRectangle {
	float x = 0.0f;
	float y = 0.0f;
	float width = 100.0f;
	float height = 100.0f;
	/// 0 keeps square corners.
	float cornerRadius = 0.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "X", EPT_FLOAT, &x},
				{1, "Y", EPT_FLOAT, &y},
				{2, "Width", EPT_FLOAT, &width},
				{3, "Height", EPT_FLOAT, &height},
				{4, "Corner Radius", EPT_FLOAT, &cornerRadius}};
	}
};

} // namespace ecs
} // namespace rigkit
