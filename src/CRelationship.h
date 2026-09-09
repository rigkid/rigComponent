#pragma once

#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Parent link for transform hierarchy (opt-in; absent = root).
 * @details NO CODE JUST DATA - no child lists (derive by scan). Walk parents
 * in rigSystems `SHierarchy` to fill `CTransform::world`. Not a document/node-editor
 * node type; keep `CNode` free for those packs. Parent is edited via hierarchy UI,
 * not the property grid.
 */
struct CRelationship {
	static constexpr bool kNoProperties = true;

	entt::entity parent{entt::null};
	int order = 0;
};

} // namespace ecs
} // namespace rigkit
