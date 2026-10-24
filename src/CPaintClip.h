#pragma once

#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Clip a drawable to a path. Speaks `rig.paint.clip`.
 */
struct CPaintClip {
	static constexpr bool kNoProperties = true;

	entt::entity path{entt::null};
};

} // namespace ecs
} // namespace rigkit
