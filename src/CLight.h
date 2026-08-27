#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ecs/PropertyReflection.h"
#include "CTransform.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Portable light POD — color/intensity + shade style.
 * @details Pose lives on `CTransform` (same entity). Speaks `rig.render.light`
 * (directional / point). Directional lights shine along local -Z (world).
 * Point lights use world translation. No GPU handles.
 * Albedo: `CMesh::faceColors` / `facePalette` / atlas, or `CDrawStyle` fill.
 */
struct CLight {
	enum class Type : int { Directional = 0, Point = 1 };

	bool enabled = true;
	Type type = Type::Directional;
	float colorR = 1.0f;
	float colorG = 1.0f;
	float colorB = 1.0f;
	float intensity = 1.0f;
	/// Floor for banded shade (0..1). Gates how far palette ramps may walk toward
	/// black (`maxDarker ≈ (1-ambient)*(bands-1)`). Also darkest mix in the shader path.
	float ambient = 0.35f;
	/// Discrete shade steps (true = banded; false = smooth Lambert).
	bool banded = true;
	/// Band count when `banded` (clamped 2..4 in present).
	int bands = 3;
	/// Ordered (Bayer) dither on banded shade thresholds.
	bool dither = false;

	glm::vec3 color() const { return {colorR, colorG, colorB}; }

	/// World-space direction the light travels (directional). Call after `SHierarchy`.
	static glm::vec3 direction(const CTransform& transform) {
		const glm::vec3 z(transform.world[2]);
		const float len = glm::length(z);
		if (len < 1e-6f) {
			return {0.0f, -1.0f, 0.0f};
		}
		return -z / len;
	}

	/// World-space position (point). Call after `SHierarchy`.
	static glm::vec3 position(const CTransform& transform) {
		return {transform.world[3][0], transform.world[3][1], transform.world[3][2]};
	}

	std::vector<sProp> GetProperties() {
		static constexpr const char* kTypeNames[] = {"Directional", "Point"};
		return {{0, "Enabled", EPT_BOOL, &enabled},
				{1, "Type", EPT_ENUM, reinterpret_cast<int*>(&type), kTypeNames, 2},
				{2, "Color R", EPT_FLOAT, &colorR},
				{3, "Color G", EPT_FLOAT, &colorG},
				{4, "Color B", EPT_FLOAT, &colorB},
				{5, "Intensity", EPT_FLOAT, &intensity},
				{6, "Ambient", EPT_FLOAT, &ambient},
				{7, "Banded", EPT_BOOL, &banded},
				{8, "Bands", EPT_INT, &bands},
				{9, "Dither", EPT_BOOL, &dither}};
	}
};

} // namespace ecs
} // namespace rigkit
