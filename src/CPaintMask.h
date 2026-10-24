#pragma once

#include <cstdint>
#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Mask a drawable. Speaks `rig.paint.mask`.
 */
struct CPaintMask {
	static constexpr bool kNoProperties = true;

	entt::entity source{entt::null};
	enum class Channel : uint8_t { Luminance, Alpha };
	Channel channel = Channel::Luminance;
};

} // namespace ecs
} // namespace rigkit
