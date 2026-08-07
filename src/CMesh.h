#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Triangle / line mesh as plain data (positions + optional indices).
 * @details Speaks `rig.geometry.mesh`. Optional `faceColors` / `facePalette`
 * (into scene `CPalette`). Empty means use `CDrawStyle` fill. Optional `texcoords`:
 * one UV per vertex when set. Present builds flat face normals; banded shade
 * lives on `CLight` / palette ramps. With `CIndexedAtlas`, present may sample
 * texcoords (see rigRender3D).
 */
struct CMesh {
	enum class Mode : int { Triangles = 0, Lines = 1, LineStrip = 2 };

	Mode mode = Mode::Triangles;
	std::vector<glm::vec3> positions;
	std::vector<uint32_t> indices; // empty = non-indexed sequential
	/// One color per triangle (face). Ignored for line modes.
	std::vector<glm::vec4> faceColors;
	/// Palette index per triangle (into `CPalette`). Empty ⇒ use faceColors / style.
	std::vector<uint8_t> facePalette;
	/// Per-vertex UVs; empty or size == positions.size().
	std::vector<glm::vec2> texcoords;

	/// Inspector mirrors for vector sizes (refreshed in GetProperties).
	int propVertexCount = 0;
	int propIndexCount = 0;
	int propFacePaletteCount = 0;
	int propTexcoordCount = 0;

	std::vector<sProp> GetProperties() {
		static constexpr const char* kModeNames[] = {"Triangles", "Lines", "Line Strip"};
		propVertexCount = static_cast<int>(positions.size());
		propIndexCount = static_cast<int>(indices.size());
		propFacePaletteCount = static_cast<int>(facePalette.size());
		propTexcoordCount = static_cast<int>(texcoords.size());
		return {{0, "Mode", EPT_ENUM, reinterpret_cast<int*>(&mode), kModeNames, 3},
				{1, "Vertices", EPT_INT, &propVertexCount},
				{2, "Indices", EPT_INT, &propIndexCount},
				{3, "Face palette entries", EPT_INT, &propFacePaletteCount},
				{4, "Texcoords", EPT_INT, &propTexcoordCount}};
	}
};

} // namespace ecs
} // namespace rigkit
