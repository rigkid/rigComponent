#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Per-entity opacity + blend. Speaks `rig.render.blend`.
 * @details `blendMode` uses Compositing-1 tokens plus add/subtract/disabled.
 * Integer values match `PixelBlendMode` (0=normal … 17=luminosity). 18=disabled.
 */
struct CRenderBlend {
	int blendMode = 0;
	float opacity = 1.f;

	std::vector<sProp> GetProperties() {
		static const char* const kModes[] = {
			"normal",     "multiply",   "screen",      "overlay",     "darken",
			"lighten",    "add",        "subtract",    "difference",  "exclusion",
			"hard-light", "soft-light", "color-dodge", "color-burn",  "hue",
			"saturation", "color",      "luminosity",  "disabled"};
		return {{0, "Blend", EPT_ENUM, &blendMode, kModes, 19},
				{1, "Opacity", EPT_FLOAT, &opacity}};
	}
};

inline const char* renderBlendToken(int mode) {
	switch (mode) {
	case 1:
		return "multiply";
	case 2:
		return "screen";
	case 3:
		return "overlay";
	case 4:
		return "darken";
	case 5:
		return "lighten";
	case 6:
		return "add";
	case 7:
		return "subtract";
	case 8:
		return "difference";
	case 9:
		return "exclusion";
	case 10:
		return "hard-light";
	case 11:
		return "soft-light";
	case 12:
		return "color-dodge";
	case 13:
		return "color-burn";
	case 14:
		return "hue";
	case 15:
		return "saturation";
	case 16:
		return "color";
	case 17:
		return "luminosity";
	case 18:
		return "disabled";
	default:
		return "normal";
	}
}

inline int renderBlendFromToken(const std::string& token) {
	if (token == "multiply") {
		return 1;
	}
	if (token == "screen") {
		return 2;
	}
	if (token == "overlay") {
		return 3;
	}
	if (token == "darken") {
		return 4;
	}
	if (token == "lighten") {
		return 5;
	}
	if (token == "add") {
		return 6;
	}
	if (token == "subtract") {
		return 7;
	}
	if (token == "difference") {
		return 8;
	}
	if (token == "exclusion") {
		return 9;
	}
	if (token == "hard-light") {
		return 10;
	}
	if (token == "soft-light") {
		return 11;
	}
	if (token == "color-dodge") {
		return 12;
	}
	if (token == "color-burn") {
		return 13;
	}
	if (token == "hue") {
		return 14;
	}
	if (token == "saturation") {
		return 15;
	}
	if (token == "color") {
		return 16;
	}
	if (token == "luminosity") {
		return 17;
	}
	if (token == "disabled") {
		return 18;
	}
	return 0;
}

} // namespace ecs
} // namespace rigkit
