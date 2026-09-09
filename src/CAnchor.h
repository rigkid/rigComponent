#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Cuboid cell that is local (0,0,0). Speaks `rig.spatial.anchor`.
 * @details `point` is the 3×3 face, `height` the Z slice (`min` / `center` /
 * `max`), `offset` a local XY nudge after the cell. Axes stay +X +Y +Z - this
 * is an offset into the entity AABB, not a sign flip. Absent component means
 * no remap (pages stay top-left / min). Not `CFontAnchor`.
 */
struct CAnchor {
	static constexpr int kPointTopLeft = 0;
	static constexpr int kPointTopCenter = 1;
	static constexpr int kPointTopRight = 2;
	static constexpr int kPointMiddleLeft = 3;
	static constexpr int kPointCenter = 4;
	static constexpr int kPointMiddleRight = 5;
	static constexpr int kPointBottomLeft = 6;
	static constexpr int kPointBottomCenter = 7;
	static constexpr int kPointBottomRight = 8;
	static constexpr int kPointCount = 9;

	static constexpr const char* kPointIds[kPointCount] = {
		"top-left",		"top-center",  "top-right",		"middle-left", "center",
		"middle-right", "bottom-left", "bottom-center", "bottom-right"};
	static constexpr const char* kPointLabels[kPointCount] = {
		"Top Left",		"Top Center",  "Top Right",		"Middle Left", "Center",
		"Middle Right", "Bottom Left", "Bottom Center", "Bottom Right"};

	static constexpr int kHeightMin = 0;
	static constexpr int kHeightCenter = 1;
	static constexpr int kHeightMax = 2;
	static constexpr int kHeightCount = 3;

	static constexpr const char* kHeightIds[kHeightCount] = {"min", "center", "max"};
	static constexpr const char* kHeightLabels[kHeightCount] = {"Min Z", "Center Z", "Max Z"};

	int point = kPointTopLeft;
	int height = kHeightMin;
	glm::vec2 offset{0.f, 0.f};

	static const char* pointToId(int v) {
		if (v >= 0 && v < kPointCount) {
			return kPointIds[v];
		}
		return kPointIds[kPointTopLeft];
	}

	static int pointFromId(const std::string& id) {
		for (int i = 0; i < kPointCount; ++i) {
			if (id == kPointIds[i]) {
				return i;
			}
		}
		return kPointTopLeft;
	}

	static const char* heightToId(int v) {
		if (v >= 0 && v < kHeightCount) {
			return kHeightIds[v];
		}
		return kHeightIds[kHeightMin];
	}

	static int heightFromId(const std::string& id) {
		for (int i = 0; i < kHeightCount; ++i) {
			if (id == kHeightIds[i]) {
				return i;
			}
		}
		return kHeightMin;
	}

	/** @brief Top-left / min / zero offset - the Contract default, omit on the wire. */
	bool isDefault() const {
		return point == kPointTopLeft && height == kHeightMin && offset.x == 0.f && offset.y == 0.f;
	}

	/** @brief Offset from AABB min (x,y) to the named face cell. */
	glm::vec2 faceOffsetMm(const glm::vec2& size) const {
		const float hx = size.x * 0.5f;
		const float hy = size.y * 0.5f;
		switch (point) {
		case kPointTopCenter:
			return {hx, size.y};
		case kPointTopRight:
			return {size.x, size.y};
		case kPointMiddleLeft:
			return {0.f, hy};
		case kPointCenter:
			return {hx, hy};
		case kPointMiddleRight:
			return {size.x, hy};
		case kPointBottomLeft:
			return {0.f, 0.f};
		case kPointBottomCenter:
			return {hx, 0.f};
		case kPointBottomRight:
			return {size.x, 0.f};
		case kPointTopLeft:
		default:
			return {0.f, size.y};
		}
	}

	/** @brief Z of the cuboid cell between @p minZ and @p maxZ. Absent height = min. */
	float heightOffsetMm(float minZ, float maxZ) const {
		switch (height) {
		case kHeightCenter:
			return (minZ + maxZ) * 0.5f;
		case kHeightMax:
			return maxZ;
		case kHeightMin:
		default:
			return minZ;
		}
	}

	/** @brief Local origin inside [@p bmin, @p bmax], including `offset`. */
	glm::vec3 originMm(const glm::vec3& bmin, const glm::vec3& bmax) const {
		const glm::vec2 xy = faceOffsetMm({bmax.x - bmin.x, bmax.y - bmin.y}) + offset;
		return {bmin.x + xy.x, bmin.y + xy.y, heightOffsetMm(bmin.z, bmax.z)};
	}

	std::vector<sProp> GetProperties() {
		return {{0, "Point", EPT_ENUM, &point, kPointLabels, kPointCount},
				{1, "Height", EPT_ENUM, &height, kHeightLabels, kHeightCount},
				{2, "Offset", EPT_VEC2, &offset.x}};
	}
};

} // namespace ecs
} // namespace rigkit
