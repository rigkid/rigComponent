#pragma once

#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Marker refs on a stroke. Speaks `rig.paint.markers`.
 */
struct CPaintMarkers {
	static constexpr bool kNoProperties = true;

	entt::entity start{entt::null};
	entt::entity mid{entt::null};
	entt::entity end{entt::null};
};

} // namespace ecs
} // namespace rigkit
