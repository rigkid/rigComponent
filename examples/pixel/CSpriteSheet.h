#pragma once

#include <algorithm>
#include <vector>
#include "ecs/PropertyReflection.h"

/**
 * @brief Sprite-sheet layout for the pixel example — app-local, not a pack POD.
 * @details Tile grid over `CIndexedAtlas` pixels. Travels as `x.rigkit.sprite_sheet`
 * when the example registers a codec. Promote to `rig.pixel.tile_set` only if a
 * second host needs the same fields.
 */
struct CSpriteSheet {
	int tileWidth = 32;
	int tileHeight = 32;
	int tilesAcross = 4;
	int tileRows = 1;
	float fps = 8.f;
	bool loop = true;
	bool playing = false;
	int frame = 0;

	int sheetWidth() const { return (std::max)(1, tilesAcross) * (std::max)(1, tileWidth); }
	int sheetHeight() const { return (std::max)(1, tileRows) * (std::max)(1, tileHeight); }
	int frameCount() const { return (std::max)(1, tilesAcross) * (std::max)(1, tileRows); }

	void frameOrigin(int index, int& outX, int& outY) const {
		const int cols = (std::max)(1, tilesAcross);
		const int n = frameCount();
		int i = index;
		if (n > 0) {
			i %= n;
			if (i < 0) {
				i += n;
			}
		}
		outX = (i % cols) * (std::max)(1, tileWidth);
		outY = (i / cols) * (std::max)(1, tileHeight);
	}

	std::vector<sProp> GetProperties() {
		return {{0, "Tile Width", EPT_INT, &tileWidth},
				{1, "Tile Height", EPT_INT, &tileHeight},
				{2, "Columns", EPT_INT, &tilesAcross},
				{3, "Rows", EPT_INT, &tileRows},
				{4, "FPS", EPT_FLOAT, &fps},
				{5, "Loop", EPT_BOOL, &loop},
				{6, "Playing", EPT_BOOL, &playing}};
	}
};
