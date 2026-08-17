#include "MeshFaces.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <unordered_map>
#include <vector>

namespace rigkit {
namespace ecs {
namespace {

void ensureIndexed(CMesh& mesh) {
	if (!mesh.indices.empty() || mesh.mode != CMesh::Mode::Triangles) {
		return;
	}
	const uint32_t n = static_cast<uint32_t>(mesh.positions.size());
	mesh.indices.resize(n);
	for (uint32_t i = 0; i < n; ++i) {
		mesh.indices[i] = i;
	}
}

uint32_t triangleCount(const CMesh& mesh) {
	if (mesh.mode != CMesh::Mode::Triangles) {
		return 0;
	}
	if (!mesh.indices.empty()) {
		return static_cast<uint32_t>(mesh.indices.size() / 3);
	}
	return static_cast<uint32_t>(mesh.positions.size() / 3);
}

bool triVerts(const CMesh& mesh, uint32_t face, uint32_t& i0, uint32_t& i1, uint32_t& i2) {
	if (!mesh.indices.empty()) {
		const size_t base = static_cast<size_t>(face) * 3;
		if (base + 2 >= mesh.indices.size()) {
			return false;
		}
		i0 = mesh.indices[base];
		i1 = mesh.indices[base + 1];
		i2 = mesh.indices[base + 2];
	} else {
		i0 = face * 3;
		i1 = face * 3 + 1;
		i2 = face * 3 + 2;
	}
	return i0 < mesh.positions.size() && i1 < mesh.positions.size() && i2 < mesh.positions.size();
}

glm::vec3 triNormal(const CMesh& mesh, uint32_t i0, uint32_t i1, uint32_t i2) {
	const glm::vec3 n =
		glm::cross(mesh.positions[i1] - mesh.positions[i0], mesh.positions[i2] - mesh.positions[i0]);
	const float len = glm::length(n);
	if (len < 1e-8f) {
		return {0.f, 1.f, 0.f};
	}
	return n / len;
}

uint64_t edgeKey(uint32_t a, uint32_t b) {
	const uint32_t lo = a < b ? a : b;
	const uint32_t hi = a < b ? b : a;
	return (static_cast<uint64_t>(lo) << 32) | static_cast<uint64_t>(hi);
}

struct UF {
	std::vector<uint32_t> p;
	explicit UF(uint32_t n) : p(n) {
		for (uint32_t i = 0; i < n; ++i) {
			p[i] = i;
		}
	}
	uint32_t find(uint32_t x) {
		while (p[x] != x) {
			p[x] = p[p[x]];
			x = p[x];
		}
		return x;
	}
	void unite(uint32_t a, uint32_t b) { p[find(a)] = find(b); }
};

void compactUnused(CMesh& mesh) {
	if (mesh.positions.empty()) {
		return;
	}
	ensureIndexed(mesh);
	std::vector<uint8_t> used(mesh.positions.size(), 0);
	for (uint32_t idx : mesh.indices) {
		if (idx < used.size()) {
			used[idx] = 1;
		}
	}
	for (uint32_t idx : mesh.loops) {
		if (idx < used.size()) {
			used[idx] = 1;
		}
	}
	std::vector<uint32_t> remap(mesh.positions.size(), std::numeric_limits<uint32_t>::max());
	std::vector<glm::vec3> newPos;
	std::vector<glm::vec2> newUv;
	newPos.reserve(mesh.positions.size());
	const bool hasUv = mesh.texcoords.size() == mesh.positions.size();
	for (size_t i = 0; i < mesh.positions.size(); ++i) {
		if (!used[i]) {
			continue;
		}
		remap[i] = static_cast<uint32_t>(newPos.size());
		newPos.push_back(mesh.positions[i]);
		if (hasUv) {
			newUv.push_back(mesh.texcoords[i]);
		}
	}
	if (newPos.size() == mesh.positions.size()) {
		return;
	}
	for (uint32_t& idx : mesh.indices) {
		idx = remap[idx];
	}
	for (uint32_t& idx : mesh.loops) {
		idx = remap[idx];
	}
	mesh.positions = std::move(newPos);
	if (hasUv) {
		mesh.texcoords = std::move(newUv);
	} else {
		mesh.texcoords.clear();
	}
}

void weldPositions(CMesh& mesh) {
	if (mesh.positions.size() < 2) {
		return;
	}
	ensureIndexed(mesh);
	constexpr float kEps = 1e-5f;
	const float inv = 1.f / kEps;
	struct Key {
		int x, y, z;
		bool operator==(const Key& o) const { return x == o.x && y == o.y && z == o.z; }
	};
	struct Hash {
		size_t operator()(const Key& k) const {
			return (static_cast<size_t>(k.x) * 73856093u) ^ (static_cast<size_t>(k.y) * 19349663u) ^
				   (static_cast<size_t>(k.z) * 83492791u);
		}
	};
	std::unordered_map<Key, uint32_t, Hash> first;
	first.reserve(mesh.positions.size());
	std::vector<uint32_t> remap(mesh.positions.size());
	for (uint32_t i = 0; i < static_cast<uint32_t>(mesh.positions.size()); ++i) {
		const glm::vec3& p = mesh.positions[i];
		const Key key{static_cast<int>(std::lround(p.x * inv)),
					  static_cast<int>(std::lround(p.y * inv)),
					  static_cast<int>(std::lround(p.z * inv))};
		const auto it = first.find(key);
		if (it == first.end()) {
			first.emplace(key, i);
			remap[i] = i;
		} else {
			remap[i] = it->second;
		}
	}
	if (first.size() == mesh.positions.size()) {
		return;
	}
	for (uint32_t& idx : mesh.indices) {
		if (idx < remap.size()) {
			idx = remap[idx];
		}
	}
	compactUnused(mesh);
}

bool extractLoop(const CMesh& mesh, const std::vector<uint32_t>& tris, std::vector<uint32_t>& loop) {
	loop.clear();
	if (tris.empty()) {
		return false;
	}
	if (tris.size() == 1) {
		uint32_t i0 = 0, i1 = 0, i2 = 0;
		if (!triVerts(mesh, tris[0], i0, i1, i2)) {
			return false;
		}
		loop = {i0, i1, i2};
		return true;
	}

	std::unordered_map<uint64_t, int> undirected;
	struct Half {
		uint32_t a = 0;
		uint32_t b = 0;
	};
	std::vector<Half> directed;
	for (uint32_t t : tris) {
		uint32_t i0 = 0, i1 = 0, i2 = 0;
		if (!triVerts(mesh, t, i0, i1, i2)) {
			continue;
		}
		const uint32_t v[3] = {i0, i1, i2};
		for (int e = 0; e < 3; ++e) {
			const uint32_t a = v[e];
			const uint32_t b = v[(e + 1) % 3];
			undirected[edgeKey(a, b)] += 1;
			directed.push_back({a, b});
		}
	}

	std::unordered_map<uint32_t, std::vector<uint32_t>> next;
	for (const Half& h : directed) {
		if (undirected[edgeKey(h.a, h.b)] != 1) {
			continue;
		}
		next[h.a].push_back(h.b);
	}
	if (next.empty()) {
		return false;
	}

	uint32_t start = next.begin()->first;
	uint32_t cur = start;
	loop.push_back(cur);
	for (size_t guard = 0; guard < directed.size() + 2; ++guard) {
		const auto it = next.find(cur);
		if (it == next.end() || it->second.size() != 1) {
			loop.clear();
			return false;
		}
		cur = it->second[0];
		if (cur == start) {
			return loop.size() >= 3;
		}
		loop.push_back(cur);
	}
	loop.clear();
	return false;
}

void weldCoplanar(CMesh& mesh) {
	const uint32_t nTris = triangleCount(mesh);
	if (nTris == 0) {
		return;
	}
	ensureIndexed(mesh);

	float extent = 1.f;
	for (const auto& p : mesh.positions) {
		extent = std::max(extent, std::max({std::fabs(p.x), std::fabs(p.y), std::fabs(p.z)}));
	}
	const float planeEps = std::max(1e-4f, extent * 1e-5f);

	std::vector<glm::vec3> nrm(nTris, {0.f, 1.f, 0.f});
	for (uint32_t t = 0; t < nTris; ++t) {
		uint32_t i0 = 0, i1 = 0, i2 = 0;
		if (triVerts(mesh, t, i0, i1, i2)) {
			nrm[t] = triNormal(mesh, i0, i1, i2);
		}
	}

	struct EdgeHit {
		uint32_t tri = 0;
		bool set = false;
	};
	std::unordered_map<uint64_t, EdgeHit> edges;
	UF uf(nTris);
	auto coplanar = [&](uint32_t a, uint32_t b) {
		if (glm::dot(nrm[a], nrm[b]) < 0.999f) {
			return false;
		}
		uint32_t i0 = 0, i1 = 0, i2 = 0;
		uint32_t j0 = 0, j1 = 0, j2 = 0;
		if (!triVerts(mesh, a, i0, i1, i2) || !triVerts(mesh, b, j0, j1, j2)) {
			return false;
		}
		const float d = glm::dot(nrm[a], mesh.positions[j0] - mesh.positions[i0]);
		return std::fabs(d) <= planeEps;
	};

	for (uint32_t t = 0; t < nTris; ++t) {
		uint32_t i0 = 0, i1 = 0, i2 = 0;
		if (!triVerts(mesh, t, i0, i1, i2)) {
			continue;
		}
		const uint32_t v[3] = {i0, i1, i2};
		for (int e = 0; e < 3; ++e) {
			const uint64_t key = edgeKey(v[e], v[(e + 1) % 3]);
			auto& hit = edges[key];
			if (!hit.set) {
				hit.tri = t;
				hit.set = true;
			} else if (coplanar(hit.tri, t)) {
				uf.unite(hit.tri, t);
			}
		}
	}

	std::vector<std::vector<uint32_t>> groups(nTris);
	for (uint32_t t = 0; t < nTris; ++t) {
		groups[uf.find(t)].push_back(t);
	}

	const bool hasPal = mesh.facePalette.size() >= nTris;
	const bool hasCol = mesh.faceColors.size() >= nTris;
	std::vector<uint32_t> newLoops;
	std::vector<uint32_t> newSizes;
	std::vector<uint8_t> newPal;
	std::vector<glm::vec4> newCol;
	newLoops.reserve(mesh.indices.size());
	newSizes.reserve(nTris);

	for (uint32_t t = 0; t < nTris; ++t) {
		if (groups[t].empty()) {
			continue;
		}
		std::vector<uint32_t> loop;
		if (!extractLoop(mesh, groups[t], loop)) {
			for (uint32_t tri : groups[t]) {
				uint32_t i0 = 0, i1 = 0, i2 = 0;
				if (!triVerts(mesh, tri, i0, i1, i2)) {
					continue;
				}
				newLoops.push_back(i0);
				newLoops.push_back(i1);
				newLoops.push_back(i2);
				newSizes.push_back(3);
				if (hasPal) {
					newPal.push_back(mesh.facePalette[tri]);
				}
				if (hasCol) {
					newCol.push_back(mesh.faceColors[tri]);
				}
			}
			continue;
		}
		newLoops.insert(newLoops.end(), loop.begin(), loop.end());
		newSizes.push_back(static_cast<uint32_t>(loop.size()));
		const uint32_t src = groups[t][0];
		if (hasPal) {
			newPal.push_back(mesh.facePalette[src]);
		}
		if (hasCol) {
			newCol.push_back(mesh.faceColors[src]);
		}
	}

	mesh.loops = std::move(newLoops);
	mesh.loopSizes = std::move(newSizes);
	if (hasPal) {
		mesh.facePalette = std::move(newPal);
	}
	if (hasCol) {
		mesh.faceColors = std::move(newCol);
	}
}

} // namespace

uint32_t meshFaceCount(const CMesh& mesh) {
	if (!mesh.loopSizes.empty()) {
		return static_cast<uint32_t>(mesh.loopSizes.size());
	}
	return triangleCount(mesh);
}

bool meshFaceRange(const CMesh& mesh, uint32_t face, uint32_t& begin, uint32_t& count) {
	if (!mesh.loopSizes.empty()) {
		if (face >= mesh.loopSizes.size()) {
			return false;
		}
		begin = 0;
		for (uint32_t i = 0; i < face; ++i) {
			begin += mesh.loopSizes[i];
		}
		count = mesh.loopSizes[face];
		return count >= 3 && begin + count <= mesh.loops.size();
	}
	count = 3;
	begin = face * 3;
	if (!mesh.indices.empty()) {
		return begin + 2 < mesh.indices.size();
	}
	return begin + 2 < mesh.positions.size();
}

uint32_t meshFaceCorner(const CMesh& mesh, uint32_t face, uint32_t corner) {
	uint32_t begin = 0, count = 0;
	if (!meshFaceRange(mesh, face, begin, count) || corner >= count) {
		return 0;
	}
	if (!mesh.loopSizes.empty()) {
		return mesh.loops[begin + corner];
	}
	if (!mesh.indices.empty()) {
		return mesh.indices[begin + corner];
	}
	return begin + corner;
}

glm::vec3 meshFaceNormal(const CMesh& mesh, uint32_t face) {
	uint32_t begin = 0, count = 0;
	if (!meshFaceRange(mesh, face, begin, count)) {
		return {0.f, 1.f, 0.f};
	}
	glm::vec3 n{0.f};
	for (uint32_t i = 0; i < count; ++i) {
		const glm::vec3& a = mesh.positions[meshFaceCorner(mesh, face, i)];
		const glm::vec3& b = mesh.positions[meshFaceCorner(mesh, face, (i + 1) % count)];
		n.x += (a.y - b.y) * (a.z + b.z);
		n.y += (a.z - b.z) * (a.x + b.x);
		n.z += (a.x - b.x) * (a.y + b.y);
	}
	const float len = glm::length(n);
	if (len < 1e-8f) {
		const uint32_t i0 = meshFaceCorner(mesh, face, 0);
		const uint32_t i1 = meshFaceCorner(mesh, face, 1);
		const uint32_t i2 = meshFaceCorner(mesh, face, std::min(2u, count - 1));
		return triNormal(mesh, i0, i1, i2);
	}
	return n / len;
}

void meshTriangulate(CMesh& mesh) {
	if (mesh.mode != CMesh::Mode::Triangles || mesh.loopSizes.empty()) {
		return;
	}
	mesh.indices.clear();
	uint32_t off = 0;
	for (uint32_t n : mesh.loopSizes) {
		if (n < 3 || off + n > mesh.loops.size()) {
			off += n;
			continue;
		}
		const uint32_t i0 = mesh.loops[off];
		for (uint32_t k = 1; k + 1 < n; ++k) {
			mesh.indices.push_back(i0);
			mesh.indices.push_back(mesh.loops[off + k]);
			mesh.indices.push_back(mesh.loops[off + k + 1]);
		}
		off += n;
	}
}

uint32_t meshTriangleToFace(const CMesh& mesh, uint32_t triangle) {
	if (mesh.loopSizes.empty()) {
		return triangle;
	}
	uint32_t tri = 0;
	for (uint32_t f = 0; f < mesh.loopSizes.size(); ++f) {
		const uint32_t n = mesh.loopSizes[f];
		const uint32_t fans = n >= 3 ? n - 2 : 0;
		if (triangle < tri + fans) {
			return f;
		}
		tri += fans;
	}
	return triangle;
}

void meshEnsureLoops(CMesh& mesh) {
	if (mesh.mode != CMesh::Mode::Triangles || !mesh.loopSizes.empty()) {
		return;
	}
	ensureIndexed(mesh);
	const uint32_t n = triangleCount(mesh);
	mesh.loops.clear();
	mesh.loopSizes.clear();
	mesh.loops.reserve(static_cast<size_t>(n) * 3);
	mesh.loopSizes.resize(n, 3);
	for (uint32_t t = 0; t < n; ++t) {
		uint32_t i0 = 0, i1 = 0, i2 = 0;
		if (!triVerts(mesh, t, i0, i1, i2)) {
			mesh.loops.push_back(0);
			mesh.loops.push_back(0);
			mesh.loops.push_back(0);
			continue;
		}
		mesh.loops.push_back(i0);
		mesh.loops.push_back(i1);
		mesh.loops.push_back(i2);
	}
}

void meshFinishFaces(CMesh& mesh) {
	if (mesh.mode != CMesh::Mode::Triangles || mesh.positions.empty()) {
		return;
	}
	if (mesh.loopSizes.empty()) {
		weldPositions(mesh);
		weldCoplanar(mesh);
	}
	meshTriangulate(mesh);
}

uint32_t meshFaceEdgeCount(const CMesh& mesh, uint32_t face) {
	uint32_t begin = 0;
	uint32_t count = 0;
	if (!meshFaceRange(mesh, face, begin, count)) {
		return 0;
	}
	return count;
}

bool meshFaceEdge(const CMesh& mesh, uint32_t face, uint32_t edge, uint32_t& a, uint32_t& b) {
	uint32_t begin = 0;
	uint32_t count = 0;
	if (!meshFaceRange(mesh, face, begin, count) || edge >= count) {
		return false;
	}
	a = meshFaceCorner(mesh, face, edge);
	b = meshFaceCorner(mesh, face, (edge + 1) % count);
	if (a > b) {
		std::swap(a, b);
	}
	return true;
}

} // namespace ecs
} // namespace rigkit
