#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief One OpenType axis value - nested type of `rig.media.text`.
 * @details `tag` is a 4-char OpenType tag packed as big-endian uint32
 * (e.g. 'wght' = 0x77676874). Min/max/name come from the font file at
 * fulfillment time; only the authored value is stored here.
 */
struct FontAxisValue {
	uint32_t tag = 0;
	float value = 0.f;
};

/**
 * @brief Scene / canvas text run - data only. Speaks `rig.media.text`.
 * @details Portable: `text`, `font` (`rig.media.asset_ref`, kind font),
 * `fontSize`, `rgba`, `axes`, `features`, `useKerning`. Never store Face*,
 * glyph caches, or GPU handles here - those live in the font fulfillment
 * pack (rigVarFont). Plot bake-to-path text stays on `CTexts`
 * (rigPlotComponent).
 */
struct CText {
	std::string text;
	/// Entity with `CAssetRef` (kind font). `entt::null` = host default face.
	entt::entity font{entt::null};
	float fontSize = 24.f;
	glm::vec4 rgba{1.f, 1.f, 1.f, 1.f};
	std::vector<FontAxisValue> axes;
	std::string features; ///< e.g. "liga,ss01" - optional OT features string.
	bool useKerning = true;
	float tracking = 0.f; ///< thousandths of an em
	enum class Anchor : uint8_t { Start, Middle, End };
	enum class WritingMode : uint8_t { HorizontalTb, VerticalRl, VerticalLr };
	enum class Direction : uint8_t { Ltr, Rtl };
	enum class Baseline : uint8_t { Auto, Alphabetic, Middle, Hanging, Ideographic, Central };
	Anchor anchor = Anchor::Start;
	WritingMode writingMode = WritingMode::HorizontalTb;
	Direction direction = Direction::Ltr;
	Baseline baseline = Baseline::Auto;
	struct Placement {
		int index = 0;
		float x = 0.f;
		float y = 0.f;
		float dx = 0.f;
		float dy = 0.f;
		float rotateDegrees = 0.f;
		bool hasX = false;
		bool hasY = false;
	};
	std::vector<Placement> placements;

	std::vector<sProp> GetProperties() {
		return {{0, "Text", EPT_STRING, &text},
				{1, "Font Size", EPT_FLOAT, &fontSize},
				{2, "Color", EPT_COLOR, &rgba.x},
				{3, "Features", EPT_STRING, &features},
				{4, "Kerning", EPT_BOOL, &useKerning},
				{5, "Tracking", EPT_FLOAT, &tracking}};
	}
};

} // namespace ecs
} // namespace rigkit
