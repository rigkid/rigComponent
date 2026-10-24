#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Marker tile. Speaks `rig.paint.marker`.
 */
struct CPaintMarker {
	float width = 0.f;
	float height = 0.f;
	glm::vec2 ref{0.f, 0.f};
	enum class Orient : uint8_t { Auto, AutoStartReverse, Angle };
	enum class Units : uint8_t { StrokeWidth, UserSpace };
	Orient orient = Orient::Auto;
	float orientDegrees = 0.f;
	Units units = Units::StrokeWidth;

	std::vector<sProp> GetProperties() {
		return {{0, "Width", EPT_FLOAT, &width},
				{1, "Height", EPT_FLOAT, &height},
				{2, "Orient Degrees", EPT_FLOAT, &orientDegrees}};
	}
};

} // namespace ecs
} // namespace rigkit
