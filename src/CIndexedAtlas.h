#pragma once

#include <cmath>
#include <cstdint>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Indexed color atlas - one palette slot per texel (lo-res paint style).
 * @details Indices reference scene `CPalette` (0..15). No GPU handles; present
 * expands to RGBA when sampling. Default size 128x128.
 */
struct CIndexedAtlas {
	int width = 128;
	int height = 128;
	/// Row-major; size should be width * height. Values are palette indices.
	std::vector<uint8_t> indices;

	void resize(int w, int h, uint8_t fillIndex = 0) {
		width = w > 0 ? w : 1;
		height = h > 0 ? h : 1;
		indices.assign(static_cast<size_t>(width * height), fillIndex);
	}

	uint8_t sample(float u, float v) const {
		if (indices.empty() || width <= 0 || height <= 0) {
			return 0;
		}
		// Wrap; v often stored OpenGL-style (0 at bottom) - present may flip.
		float wu = u - std::floor(u);
		float wv = v - std::floor(v);
		if (wu < 0.f) {
			wu += 1.f;
		}
		if (wv < 0.f) {
			wv += 1.f;
		}
		int x = static_cast<int>(wu * static_cast<float>(width));
		int y = static_cast<int>(wv * static_cast<float>(height));
		if (x >= width) {
			x = width - 1;
		}
		if (y >= height) {
			y = height - 1;
		}
		return indices[static_cast<size_t>(y * width + x)];
	}

	void stamp(int x, int y, uint8_t paletteIndex, int radius = 0) {
		if (indices.empty() || width <= 0 || height <= 0) {
			return;
		}
		const int r = radius < 0 ? 0 : radius;
		for (int dy = -r; dy <= r; ++dy) {
			for (int dx = -r; dx <= r; ++dx) {
				if (dx * dx + dy * dy > r * r) {
					continue;
				}
				const int px = x + dx;
				const int py = y + dy;
				if (px < 0 || py < 0 || px >= width || py >= height) {
					continue;
				}
				indices[static_cast<size_t>(py * width + px)] = paletteIndex & 15;
			}
		}
	}

	std::vector<sProp> GetProperties() {
		return {{0, "Width", EPT_INT, &width}, {1, "Height", EPT_INT, &height}};
	}
};

} // namespace ecs
} // namespace rigkit
