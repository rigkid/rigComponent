#pragma once

#include <cstdint>
#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Filter pointer or graph root. Speaks `rig.paint.filter`.
 */
struct CPaintFilter {
	static constexpr bool kNoProperties = true;

	entt::entity filter{entt::null};
	enum class Units : uint8_t { UserSpace, Object };
	Units primitiveUnits = Units::UserSpace;
};

} // namespace ecs
} // namespace rigkit
