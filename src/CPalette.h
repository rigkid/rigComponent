#pragma once

#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Portable indexed colour table. Speaks `rig.pixel.palette`.
 * @details Contract field is `colors` (rgba[]). `shadeNext` is a host
 * extension for shade ramps. Faces may index this via `CMesh::facePalette`.
 * No GPU handles.
 */
struct CPalette {
	static constexpr int kCount = 16;

	std::array<glm::vec4, kCount> colors{};
	/// Next darker palette index for shade ramps; -1 = stop.
	std::array<int, kCount> shadeNext{};

	CPalette() {
		for (int i = 0; i < kCount; ++i) {
			colors[static_cast<size_t>(i)] = {1.f, 1.f, 1.f, 1.f};
			shadeNext[static_cast<size_t>(i)] = -1;
		}
	}

	glm::vec4 color(int index) const {
		return colors[static_cast<size_t>(index & (kCount - 1))];
	}

	/** @brief Walk `shadeNext` `darkerSteps` times from `index`. */
	glm::vec4 shaded(int index, int darkerSteps) const {
		int i = index & (kCount - 1);
		for (int n = 0; n < darkerSteps; ++n) {
			const int next = shadeNext[static_cast<size_t>(i)];
			if (next < 0 || next == i) {
				break;
			}
			i = next & (kCount - 1);
		}
		return colors[static_cast<size_t>(i)];
	}

	/** @brief Default 16-slot palette with simple ramps toward black/grey. */
	static CPalette default16() {
		CPalette p;
		const glm::vec4 k[kCount] = {
			{0.00f, 0.00f, 0.00f, 1.f}, {0.11f, 0.17f, 0.33f, 1.f}, {0.49f, 0.15f, 0.33f, 1.f},
			{0.00f, 0.53f, 0.32f, 1.f}, {0.67f, 0.32f, 0.21f, 1.f}, {0.37f, 0.34f, 0.31f, 1.f},
			{0.76f, 0.76f, 0.78f, 1.f}, {1.00f, 0.95f, 0.91f, 1.f}, {1.00f, 0.00f, 0.30f, 1.f},
			{1.00f, 0.64f, 0.00f, 1.f}, {1.00f, 0.93f, 0.15f, 1.f}, {0.00f, 0.89f, 0.21f, 1.f},
			{0.16f, 0.68f, 1.00f, 1.f}, {0.51f, 0.46f, 0.61f, 1.f}, {1.00f, 0.47f, 0.66f, 1.f},
			{1.00f, 0.80f, 0.67f, 1.f},
		};
		for (int i = 0; i < kCount; ++i) {
			p.colors[static_cast<size_t>(i)] = k[i];
			p.shadeNext[static_cast<size_t>(i)] = 0;
		}
		p.shadeNext[0] = -1;
		p.shadeNext[7] = 6;
		p.shadeNext[6] = 5;
		p.shadeNext[5] = -1; // stop on dark grey - avoid crushing to hard black
		p.shadeNext[8] = 2;
		p.shadeNext[2] = 5;
		p.shadeNext[9] = 4;
		p.shadeNext[4] = 5;
		p.shadeNext[10] = 9;
		p.shadeNext[11] = 3;
		p.shadeNext[3] = 5;
		p.shadeNext[12] = 1;
		p.shadeNext[1] = 5;
		p.shadeNext[13] = 5;
		p.shadeNext[14] = 2;
		p.shadeNext[15] = 4;
		return p;
	}

	std::vector<sProp> GetProperties() {
		// Editor shows first few slots; full table is data for present/tools.
		return {{0, "Color0 R", EPT_FLOAT, &colors[0].r},
				{1, "Color0 G", EPT_FLOAT, &colors[0].g},
				{2, "Color0 B", EPT_FLOAT, &colors[0].b},
				{3, "Color7 R", EPT_FLOAT, &colors[7].r},
				{4, "Color7 G", EPT_FLOAT, &colors[7].g},
				{5, "Color7 B", EPT_FLOAT, &colors[7].b}};
	}
};

} // namespace ecs
} // namespace rigkit
