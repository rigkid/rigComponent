#pragma once

namespace rigkit {
namespace ecs {

/**
 * @brief Scene group marker. Speaks `rig.spatial.group`.
 * @details Rig schema has no fields; `_` is host storage only (EnTT needs a
 * non-empty type). Parenting stays on `CRelationship`; pose on `CTransform`.
 */
struct CGroup {
	static constexpr bool kNoProperties = true;

	uint8_t _ = 0;
};

} // namespace ecs
} // namespace rigkit
