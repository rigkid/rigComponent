#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Mesh as plain data: shared verts + optional n-gon faces.
 * @details Speaks `rig.geometry.mesh`. `loops` / `loopSizes` are the authored
 * faces (a cube is 6 quads, not 12 triangles). Edges are consecutive pairs in
 * each loop (plus close) — not a second table. Empty loops = legacy triangles
 * from `indices`. Present tessellates loops to `indices`. Optional `faceColors`
 * / `facePalette` are one entry per authored face. Empty means use `CDrawStyle`
 * fill. Optional `texcoords`: one UV per vertex when set.
 */
struct CMesh {
	enum class Mode : int { Triangles = 0, Lines = 1, LineStrip = 2 };

	Mode mode = Mode::Triangles;
	std::vector<glm::vec3> positions;
	std::vector<uint32_t> indices; // empty = non-indexed sequential
	/// Concatenated n-gon vertex indices. Empty = faces are implicit triangles.
	std::vector<uint32_t> loops;
	/// Vertex count per face; sum equals `loops.size()`.
	std::vector<uint32_t> loopSizes;
	/// One color per authored face. Ignored for line modes.
	std::vector<glm::vec4> faceColors;
	/// Palette index per authored face (into `CPalette`). Empty ⇒ use faceColors / style.
	std::vector<uint8_t> facePalette;
	/// Per-vertex UVs; empty or size == positions.size().
	std::vector<glm::vec2> texcoords;

	/// Inspector mirrors for vector sizes (refreshed in GetProperties).
	int propVertexCount = 0;
	int propIndexCount = 0;
	int propFaceCount = 0;
	int propFacePaletteCount = 0;
	int propTexcoordCount = 0;

	std::vector<sProp> GetProperties() {
		static constexpr const char* kModeNames[] = {"Triangles", "Lines", "Line Strip"};
		propVertexCount = static_cast<int>(positions.size());
		propIndexCount = static_cast<int>(indices.size());
		propFaceCount = loopSizes.empty()
							? (indices.empty() ? static_cast<int>(positions.size() / 3)
											   : static_cast<int>(indices.size() / 3))
							: static_cast<int>(loopSizes.size());
		propFacePaletteCount = static_cast<int>(facePalette.size());
		propTexcoordCount = static_cast<int>(texcoords.size());
		return {{0, "Mode", EPT_ENUM, reinterpret_cast<int*>(&mode), kModeNames, 3},
				{1, "Vertices", EPT_INT, &propVertexCount},
				{2, "Faces", EPT_INT, &propFaceCount},
				{3, "Indices", EPT_INT, &propIndexCount},
				{4, "Face palette entries", EPT_INT, &propFacePaletteCount},
				{5, "Texcoords", EPT_INT, &propTexcoordCount}};
	}
};

} // namespace ecs
} // namespace rigkit
