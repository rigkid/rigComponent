#pragma once

#include <cstdint>
#include <entt/entt.hpp>
#include <string>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Named compiled face. Speaks `rig.media.font_face`.
 */
struct CFontFace {
	std::string family;
	entt::entity asset{entt::null};
	float weight = 400.f;
	enum class Style : uint8_t { Normal, Italic };
	Style style = Style::Normal;

	std::vector<sProp> GetProperties() {
		return {{0, "Family", EPT_STRING, &family}, {1, "Weight", EPT_FLOAT, &weight}};
	}
};

} // namespace ecs
} // namespace rigkit
