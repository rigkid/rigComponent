#pragma once

#include <cstdint>
#include <entt/entt.hpp>

namespace rigkit {
namespace ecs {

/**
 * @brief Bind a canvas run to a baseline contour. Speaks `rig.media.text_path`.
 */
struct CTextPath {
	static constexpr bool kNoProperties = true;

	entt::entity path{entt::null};
	float startOffset = 0.f;
	enum class OffsetMode : uint8_t { Length, Ratio };
	enum class Method : uint8_t { Align, Stretch };
	enum class Spacing : uint8_t { Auto, Exact };
	enum class Side : uint8_t { Left, Right };
	OffsetMode startOffsetMode = OffsetMode::Length;
	Method method = Method::Align;
	Spacing spacing = Spacing::Exact;
	Side side = Side::Left;
};

} // namespace ecs
} // namespace rigkit
