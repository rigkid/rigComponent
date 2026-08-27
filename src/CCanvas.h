#pragma once

#include <string>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Canvas surface POD (size, clear color, grid, clock).
 * @details Clock fields (`time` / `frameCount`) are advanced by SCanvasUpdate.
 * Not serialized by rigProject yet (no codec) - stays non-portable until that lands.
 */
struct CCanvas {
	int width = 1024;
	int height = 768;
	std::string name = "Canvas";
	float backgroundR = 1.0f, backgroundG = 1.0f, backgroundB = 1.0f, backgroundA = 1.0f;
	bool showGrid = true;
	float gridSpacing = 32.0f;
	/// Seconds since setup; advanced by SCanvasUpdate.
	float time = 0.0f;
	/// Frames since setup; advanced by SCanvasUpdate.
	int frameCount = 0;

	std::vector<sProp> GetProperties() {
		return {{0, "Width", EPT_INT, &width},
				{1, "Height", EPT_INT, &height},
				{2, "Name", EPT_STRING, &name},
				{3, "Background R", EPT_FLOAT, &backgroundR},
				{4, "Background G", EPT_FLOAT, &backgroundG},
				{5, "Background B", EPT_FLOAT, &backgroundB},
				{6, "Background A", EPT_FLOAT, &backgroundA},
				{7, "Show Grid", EPT_BOOL, &showGrid},
				{8, "Grid Spacing", EPT_FLOAT, &gridSpacing},
				{9, "Time", EPT_FLOAT, &time},
				{10, "Frame Count", EPT_INT, &frameCount}};
	}
};

} // namespace ecs
} // namespace rigkit
