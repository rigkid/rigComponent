#pragma once

#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Instance of another entity. Speaks `rig.spatial.instance`.
 */
struct CInstance {
	static constexpr bool kNoProperties = true;

	entt::entity source{entt::null};
};

} // namespace ecs
} // namespace rigkit
