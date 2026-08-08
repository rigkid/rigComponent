#include "PrimitiveVertices3D.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace rigkit {
namespace ecs {
namespace {

using Face = std::array<glm::vec3, 3>;

/// Regular icosahedron, unit radius, as 20 loose (non-indexed) triangles.
std::vector<Face> icosahedronFaces() {
	const float t = (1.0f + std::sqrt(5.0f)) * 0.5f;
	const std::array<glm::vec3, 12> v = {
		glm::normalize(glm::vec3{-1.f, t, 0.f}), glm::normalize(glm::vec3{1.f, t, 0.f}),
		glm::normalize(glm::vec3{-1.f, -t, 0.f}), glm::normalize(glm::vec3{1.f, -t, 0.f}),
		glm::normalize(glm::vec3{0.f, -1.f, t}), glm::normalize(glm::vec3{0.f, 1.f, t}),
		glm::normalize(glm::vec3{0.f, -1.f, -t}), glm::normalize(glm::vec3{0.f, 1.f, -t}),
		glm::normalize(glm::vec3{t, 0.f, -1.f}), glm::normalize(glm::vec3{t, 0.f, 1.f}),
		glm::normalize(glm::vec3{-t, 0.f, -1.f}), glm::normalize(glm::vec3{-t, 0.f, 1.f}),
	};
	static constexpr int kIdx[20][3] = {
		{0, 11, 5}, {0, 5, 1},	{0, 1, 7},	{0, 7, 10}, {0, 10, 11}, {1, 5, 9},	 {5, 11, 4},
		{11, 10, 2}, {10, 7, 6}, {7, 1, 8},	{3, 9, 4},	{3, 4, 2},	{3, 2, 6},	 {3, 6, 8},
		{3, 8, 9},	{4, 9, 5},	{2, 4, 11}, {6, 2, 10}, {8, 6, 7},	{9, 8, 1},
	};
	std::vector<Face> faces;
	faces.reserve(20);
	for (const auto& f : kIdx) {
		faces.push_back({v[static_cast<size_t>(f[0])], v[static_cast<size_t>(f[1])],
						  v[static_cast<size_t>(f[2])]});
	}
	return faces;
}

/// Split each triangle into 4 at normalized edge midpoints (one sphere-ward step).
std::vector<Face> subdivide(const std::vector<Face>& faces) {
	std::vector<Face> out;
	out.reserve(faces.size() * 4);
	for (const auto& f : faces) {
		const glm::vec3 ab = glm::normalize(f[0] + f[1]);
		const glm::vec3 bc = glm::normalize(f[1] + f[2]);
		const glm::vec3 ca = glm::normalize(f[2] + f[0]);
		out.push_back({f[0], ab, ca});
		out.push_back({ab, f[1], bc});
		out.push_back({ca, bc, f[2]});
		out.push_back({ab, bc, ca});
	}
	return out;
}

} // namespace

std::vector<glm::vec3> verticesOf(const CSphere& shape) {
	auto faces = icosahedronFaces();
	const int steps = std::clamp(shape.subdivisions, 0, 4);
	for (int i = 0; i < steps; ++i) {
		faces = subdivide(faces);
	}

	std::vector<glm::vec3> positions;
	positions.reserve(faces.size() * 3);
	for (const auto& f : faces) {
		positions.push_back(f[0] * shape.radius);
		positions.push_back(f[1] * shape.radius);
		positions.push_back(f[2] * shape.radius);
	}
	return positions;
}

} // namespace ecs
} // namespace rigkit
