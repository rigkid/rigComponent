#pragma once

#include <cstdint>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Tiled paint. Speaks `rig.paint.pattern`.
 */
struct CPaintPattern {
	float width = 0.f;
	float height = 0.f;
	enum class Units : uint8_t { Object, UserSpace };
	Units units = Units::Object;
	Units contentUnits = Units::UserSpace;

	std::vector<sProp> GetProperties() {
		return {{0, "Width", EPT_FLOAT, &width}, {1, "Height", EPT_FLOAT, &height}};
	}
};

} // namespace ecs
} // namespace rigkit
