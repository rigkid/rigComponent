#include "PixelDoc.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <queue>
#include <utility>

#include "ecs/MEcs.h"
#include "packs/rigComponent/src/rig.h"
#include "packs/rigImGui/src/EntityPick.h"
#include "packs/rigProject/src/rigProject.h"
#include "CIndexedAtlas.h"
#include "CLayer.h"
#include "CPalette.h"
#include "CProject.h"
#include "CSelection.h"
#include "AddSerializer.h"
#include "ComponentSerializerRegistry.h"
#include "ProjectJson.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/glfw/deps/stb_image_write.h"

namespace pixel {
namespace {

constexpr int kMaxSheet = 256;

int clampSize(int v, int lo, int hi) {
	return (std::max)(lo, (std::min)(v, hi));
}

CSpriteSheet clampSpec(CSpriteSheet s) {
	s.tileWidth = clampSize(s.tileWidth, 1, kMaxSheet);
	s.tileHeight = clampSize(s.tileHeight, 1, kMaxSheet);
	s.tilesAcross = clampSize(s.tilesAcross, 1, 16);
	s.tileRows = clampSize(s.tileRows, 1, 16);
	while (s.sheetWidth() > kMaxSheet && s.tilesAcross > 1) {
		--s.tilesAcross;
	}
	while (s.sheetHeight() > kMaxSheet && s.tileRows > 1) {
		--s.tileRows;
	}
	if (s.sheetWidth() > kMaxSheet) {
		s.tileWidth = kMaxSheet / s.tilesAcross;
	}
	if (s.sheetHeight() > kMaxSheet) {
		s.tileHeight = kMaxSheet / s.tileRows;
	}
	s.fps = (std::max)(0.1f, s.fps);
	return s;
}

void put(rigkit::ecs::CIndexedAtlas& atlas, int x, int y, uint8_t index) {
	if (x < 0 || y < 0 || x >= atlas.width || y >= atlas.height || atlas.indices.empty()) {
		return;
	}
	atlas.indices[static_cast<size_t>(y * atlas.width + x)] = index & 15;
}

void stampDemo(rigkit::ecs::CIndexedAtlas& atlas, const CSpriteSheet& spec) {
	const int n = spec.frameCount();
	for (int f = 0; f < n; ++f) {
		int ox = 0, oy = 0;
		spec.frameOrigin(f, ox, oy);
		const int leg = (f & 1) ? 1 : 0;
		const int bob = (f == 1 || f == 3) ? -1 : 0;
		const int cx = ox + spec.tileWidth / 2 - 3;
		const int cy = oy + spec.tileHeight / 2 - 6 + bob;
		// Head
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 6; ++x) {
				put(atlas, cx + x, cy + y, 15);
			}
		}
		put(atlas, cx + 1, cy + 1, 0);
		put(atlas, cx + 4, cy + 1, 0);
		// Body
		for (int y = 4; y < 9; ++y) {
			for (int x = 1; x < 5; ++x) {
				put(atlas, cx + x, cy + y, 12);
			}
		}
		// Legs
		put(atlas, cx + 1, cy + 9, 1);
		put(atlas, cx + 1, cy + 10, 1);
		put(atlas, cx + 4, cy + 9, 1);
		put(atlas, cx + 4 + leg, cy + 10, 1);
		put(atlas, cx + 4 - leg, cy + 11, 1);
	}
}

void stampBox(rigkit::ecs::CIndexedAtlas& atlas, int x, int y, int size, uint8_t index) {
	const int r = (std::max)(0, size / 2);
	for (int dy = -r; dy <= r; ++dy) {
		for (int dx = -r; dx <= r; ++dx) {
			put(atlas, x + dx, y + dy, index);
		}
	}
}

void floodFill(rigkit::ecs::CIndexedAtlas& atlas, int x, int y, uint8_t to) {
	if (atlas.indices.empty() || atlas.width <= 0 || atlas.height <= 0) {
		return;
	}
	if (x < 0 || y < 0 || x >= atlas.width || y >= atlas.height) {
		return;
	}
	const uint8_t from = atlas.indices[static_cast<size_t>(y * atlas.width + x)];
	to = to & 15;
	if (from == to) {
		return;
	}
	std::queue<std::pair<int, int>> q;
	q.push({x, y});
	while (!q.empty()) {
		const auto [px, py] = q.front();
		q.pop();
		if (px < 0 || py < 0 || px >= atlas.width || py >= atlas.height) {
			continue;
		}
		uint8_t& cell = atlas.indices[static_cast<size_t>(py * atlas.width + px)];
		if (cell != from) {
			continue;
		}
		cell = to;
		q.push({px + 1, py});
		q.push({px - 1, py});
		q.push({px, py + 1});
		q.push({px, py - 1});
	}
}

glm::vec4 paletteColor(rigkit::MEcs& ecs, int index) {
	const auto pal = findPalette(ecs);
	if (pal != entt::null && ecs.hasComponent<rigkit::ecs::CPalette>(pal)) {
		return ecs.getComponent<rigkit::ecs::CPalette>(pal).color(index);
	}
	return {1.f, 1.f, 1.f, 1.f};
}

void writePixel(std::vector<uint8_t>& rgba, int w, int x, int y, const glm::vec4& c) {
	if (c.a <= 0.001f) {
		return;
	}
	const size_t i = static_cast<size_t>((y * w + x) * 4);
	if (i + 3 >= rgba.size()) {
		return;
	}
	const float srcA = c.a;
	const float dstA = rgba[i + 3] / 255.f;
	const float outA = srcA + dstA * (1.f - srcA);
	auto mix = [&](float src, uint8_t dst) {
		const float d = dst / 255.f;
		const float o = (src * srcA + d * dstA * (1.f - srcA)) / (outA > 1e-6f ? outA : 1.f);
		return static_cast<uint8_t>(std::lround(o * 255.f));
	};
	rgba[i + 0] = mix(c.r, rgba[i + 0]);
	rgba[i + 1] = mix(c.g, rgba[i + 1]);
	rgba[i + 2] = mix(c.b, rgba[i + 2]);
	rgba[i + 3] = static_cast<uint8_t>(std::lround(outA * 255.f));
}

bool serializeSheet(entt::registry& reg, entt::entity e, rigkit::project::ordered_json& j) {
	if (!reg.all_of<CSpriteSheet>(e)) {
		return false;
	}
	const auto& s = reg.get<CSpriteSheet>(e);
	j["tileWidth"] = s.tileWidth;
	j["tileHeight"] = s.tileHeight;
	j["tilesAcross"] = s.tilesAcross;
	j["tileRows"] = s.tileRows;
	j["fps"] = s.fps;
	j["loop"] = s.loop;
	return true;
}

bool deserializeSheet(entt::registry& reg, entt::entity e, const rigkit::project::ordered_json& j) {
	CSpriteSheet s;
	s.tileWidth = j.value("tileWidth", s.tileWidth);
	s.tileHeight = j.value("tileHeight", s.tileHeight);
	s.tilesAcross = j.value("tilesAcross", s.tilesAcross);
	s.tileRows = j.value("tileRows", s.tileRows);
	s.fps = j.value("fps", s.fps);
	s.loop = j.value("loop", s.loop);
	reg.emplace_or_replace<CSpriteSheet>(e, clampSpec(s));
	return true;
}

} // namespace

entt::entity findSheet(rigkit::MEcs& ecs) {
	auto named = ecs.findEntity("sheet");
	if (named != entt::null && ecs.hasComponent<CSpriteSheet>(named)) {
		return named;
	}
	for (auto e : ecs.view<CSpriteSheet>()) {
		return e;
	}
	return entt::null;
}

entt::entity findPalette(rigkit::MEcs& ecs) {
	auto named = ecs.findEntity("palette");
	if (named != entt::null && ecs.hasComponent<rigkit::ecs::CPalette>(named)) {
		return named;
	}
	for (auto e : ecs.view<rigkit::ecs::CPalette>()) {
		return e;
	}
	return entt::null;
}

entt::entity findProject(rigkit::MEcs& ecs) {
	for (auto e : ecs.view<rigkit::ecs::CProject>()) {
		return e;
	}
	return entt::null;
}

std::vector<entt::entity> layersByOrder(rigkit::MEcs& ecs) {
	struct Row {
		entt::entity e;
		int order;
	};
	std::vector<Row> rows;
	for (auto e : ecs.view<rigkit::ecs::CLayer, rigkit::ecs::CIndexedAtlas>()) {
		rows.push_back({e, ecs.getComponent<rigkit::ecs::CLayer>(e).order});
	}
	std::sort(rows.begin(), rows.end(),
			  [](const Row& a, const Row& b) { return a.order < b.order; });
	std::vector<entt::entity> out;
	out.reserve(rows.size());
	for (const auto& r : rows) {
		out.push_back(r.e);
	}
	return out;
}

entt::entity selectedLayer(rigkit::MEcs& ecs) {
	for (auto e : ecs.view<rigkit::ecs::CSelection, rigkit::ecs::CIndexedAtlas>()) {
		const auto& sel = ecs.getComponent<rigkit::ecs::CSelection>(e);
		if (sel.isSelected || sel.isMultiSelected) {
			return e;
		}
	}
	const auto layers = layersByOrder(ecs);
	return layers.empty() ? entt::null : layers.back();
}

void registerSerializers(rigkit::rigProject& doc) {
	auto& registry = doc.serializer().registry();
	rigkit::project::addSerializer<CSpriteSheet>(registry, "SpriteSheet", "x.rigkit.sprite_sheet",
												 serializeSheet, deserializeSheet);
	// CLayer travels as rig.spatial.layer (+ x.rigkit.layer_visible) from rigComponent.
}


void markDirty(rigkit::MEcs& ecs) {
	const auto proj = findProject(ecs);
	if (proj != entt::null) {
		ecs.getComponent<rigkit::ecs::CProject>(proj).dirty = true;
	}
}

void newDocument(rigkit::MEcs& ecs, const CSpriteSheet& specIn, bool stamp) {
	const CSpriteSheet spec = clampSpec(specIn);
	ecs.clear();

	auto doc = ecs.createEntity("document");
	rigkit::ecs::CProject project;
	project.title = "Sprite Sheet";
	project.dirty = true;
	ecs.addComponent(doc, project);

	rig::makePalette(ecs, rigkit::ecs::CPalette::default16(), "palette");

	auto sheet = ecs.createEntity("sheet");
	ecs.addComponent(sheet, spec);

	auto layer = addLayer(ecs, "Layer 1");
	if (stamp && layer != entt::null) {
		stampDemo(ecs.getComponent<rigkit::ecs::CIndexedAtlas>(layer), spec);
	}
}

entt::entity addLayer(rigkit::MEcs& ecs, const std::string& name) {
	int w = 32;
	int h = 32;
	int order = 0;
	if (const auto sheet = findSheet(ecs); sheet != entt::null) {
		const auto& s = ecs.getComponent<CSpriteSheet>(sheet);
		w = s.sheetWidth();
		h = s.sheetHeight();
	}
	for (auto e : ecs.view<rigkit::ecs::CLayer>()) {
		order = (std::max)(order, ecs.getComponent<rigkit::ecs::CLayer>(e).order + 1);
	}
	for (auto e : ecs.view<rigkit::ecs::CIndexedAtlas>()) {
		const auto& a = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e);
		if (a.width > 0 && a.height > 0) {
			w = a.width;
			h = a.height;
			break;
		}
	}

	auto layer = rig::makeIndexedAtlas(ecs, w, h, 0, name);
	rigkit::ecs::CLayer tag;
	tag.order = order;
	ecs.addComponent(layer, tag);
	rigkit::selectEntityOnly(ecs, layer);
	markDirty(ecs);
	return layer;
}

void deleteSelectedLayer(rigkit::MEcs& ecs) {
	destroyLayer(ecs, selectedLayer(ecs));
}

void ensureAfterLoad(rigkit::MEcs& ecs) {
	int order = 0;
	for (auto e : ecs.view<rigkit::ecs::CIndexedAtlas>()) {
		if (!ecs.hasComponent<rigkit::ecs::CLayer>(e)) {
			rigkit::ecs::CLayer tag;
			tag.order = order++;
			ecs.addComponent(e, tag);
		}
	}
	if (findSheet(ecs) == entt::null) {
		CSpriteSheet spec;
		for (auto e : ecs.view<rigkit::ecs::CIndexedAtlas>()) {
			const auto& a = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e);
			spec.tileWidth = a.width > 0 ? a.width : 32;
			spec.tileHeight = a.height > 0 ? a.height : 32;
			spec.tilesAcross = 1;
			spec.tileRows = 1;
			break;
		}
		auto sheet = ecs.createEntity("sheet");
		ecs.addComponent(sheet, clampSpec(spec));
	}
	if (findPalette(ecs) == entt::null) {
		rig::makePalette(ecs, rigkit::ecs::CPalette::default16(), "palette");
	}
}

void paint(rigkit::MEcs& ecs, int x, int y, const ToolState& tool) {
	const auto layer = selectedLayer(ecs);
	if (layer == entt::null) {
		return;
	}
	if (ecs.hasComponent<rigkit::ecs::CLayer>(layer) &&
		ecs.getComponent<rigkit::ecs::CLayer>(layer).locked) {
		return;
	}
	auto& atlas = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(layer);
	const uint8_t index =
		tool.tool == Tool::Eraser ? uint8_t{0} : static_cast<uint8_t>(tool.color & 15);
	if (tool.tool == Tool::Fill) {
		floodFill(atlas, x, y, index);
	} else if (tool.shape == BrushShape::Circle) {
		atlas.stamp(x, y, index, (std::max)(0, tool.size / 2));
	} else {
		stampBox(atlas, x, y, tool.size, index);
	}
	markDirty(ecs);
}

bool captureLayer(rigkit::MEcs& ecs, entt::entity e, LayerSnap& out) {
	if (e == entt::null || !ecs.registry().valid(e) ||
		!ecs.hasComponent<rigkit::ecs::CIndexedAtlas>(e)) {
		return false;
	}
	out.name = ecs.entityName(e);
	out.atlas = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e);
	if (ecs.hasComponent<rigkit::ecs::CLayer>(e)) {
		out.layer = ecs.getComponent<rigkit::ecs::CLayer>(e);
	} else {
		out.layer = {};
	}
	return true;
}

entt::entity restoreLayer(rigkit::MEcs& ecs, const LayerSnap& snap) {
	auto e = rig::makeIndexedAtlas(ecs, snap.atlas.width, snap.atlas.height, 0, snap.name);
	ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e) = snap.atlas;
	if (ecs.hasComponent<rigkit::ecs::CLayer>(e)) {
		ecs.getComponent<rigkit::ecs::CLayer>(e) = snap.layer;
	} else {
		ecs.addComponent(e, snap.layer);
	}
	rigkit::selectEntityOnly(ecs, e);
	markDirty(ecs);
	return e;
}

void destroyLayer(rigkit::MEcs& ecs, entt::entity e) {
	if (e == entt::null || !ecs.registry().valid(e)) {
		return;
	}
	if (layersByOrder(ecs).size() <= 1) {
		return;
	}
	ecs.destroyEntity(e);
	const auto remain = layersByOrder(ecs);
	if (!remain.empty()) {
		rigkit::selectEntityOnly(ecs, remain.back());
	}
	markDirty(ecs);
}

void applyAtlas(rigkit::MEcs& ecs, entt::entity e, const std::string& name, int w, int h,
				const std::vector<uint8_t>& indices) {
	if (e == entt::null || !ecs.registry().valid(e) ||
		!ecs.hasComponent<rigkit::ecs::CIndexedAtlas>(e)) {
		e = ecs.findEntity(name);
	}
	if (e == entt::null || !ecs.hasComponent<rigkit::ecs::CIndexedAtlas>(e)) {
		return;
	}
	auto& atlas = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e);
	atlas.width = w;
	atlas.height = h;
	atlas.indices = indices;
	markDirty(ecs);
}

void compositeSheet(rigkit::MEcs& ecs, std::vector<uint8_t>& rgba, int& w, int& h) {
	w = 0;
	h = 0;
	if (const auto sheet = findSheet(ecs); sheet != entt::null) {
		const auto& s = ecs.getComponent<CSpriteSheet>(sheet);
		w = s.sheetWidth();
		h = s.sheetHeight();
	}
	for (auto e : ecs.view<rigkit::ecs::CIndexedAtlas>()) {
		const auto& a = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e);
		w = (std::max)(w, a.width);
		h = (std::max)(h, a.height);
	}
	if (w <= 0 || h <= 0) {
		rgba.clear();
		return;
	}
	rgba.assign(static_cast<size_t>(w * h * 4), 0);
	for (auto e : layersByOrder(ecs)) {
		const auto& layer = ecs.getComponent<rigkit::ecs::CLayer>(e);
		if (!layer.visible) {
			continue;
		}
		const auto& atlas = ecs.getComponent<rigkit::ecs::CIndexedAtlas>(e);
		const int cw = (std::min)(w, atlas.width);
		const int ch = (std::min)(h, atlas.height);
		for (int y = 0; y < ch; ++y) {
			for (int x = 0; x < cw; ++x) {
				const uint8_t idx = atlas.indices[static_cast<size_t>(y * atlas.width + x)];
				if (idx == 0) {
					continue;
				}
				auto c = paletteColor(ecs, idx);
				c.a *= layer.colorA;
				writePixel(rgba, w, x, y, c);
			}
		}
	}
}

void compositeFrame(rigkit::MEcs& ecs, int frame, std::vector<uint8_t>& rgba, int& w, int& h) {
	std::vector<uint8_t> sheet;
	int sw = 0, sh = 0;
	compositeSheet(ecs, sheet, sw, sh);
	CSpriteSheet spec;
	if (const auto e = findSheet(ecs); e != entt::null) {
		spec = ecs.getComponent<CSpriteSheet>(e);
	}
	w = (std::max)(1, spec.tileWidth);
	h = (std::max)(1, spec.tileHeight);
	int ox = 0, oy = 0;
	spec.frameOrigin(frame, ox, oy);
	rgba.assign(static_cast<size_t>(w * h * 4), 0);
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			const int sx = ox + x;
			const int sy = oy + y;
			if (sx < 0 || sy < 0 || sx >= sw || sy >= sh) {
				continue;
			}
			const size_t si = static_cast<size_t>((sy * sw + sx) * 4);
			const size_t di = static_cast<size_t>((y * w + x) * 4);
			rgba[di + 0] = sheet[si + 0];
			rgba[di + 1] = sheet[si + 1];
			rgba[di + 2] = sheet[si + 2];
			rgba[di + 3] = sheet[si + 3];
		}
	}
}

bool exportImage(const std::string& path, const uint8_t* rgba, int w, int h) {
	if (!rgba || w <= 0 || h <= 0 || path.empty()) {
		return false;
	}
	std::string ext;
	const auto dot = path.find_last_of('.');
	if (dot != std::string::npos) {
		ext = path.substr(dot);
		for (char& c : ext) {
			c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		}
	}
	if (ext == ".bmp") {
		return stbi_write_bmp(path.c_str(), w, h, 4, rgba) != 0;
	}
	return stbi_write_png(path.c_str(), w, h, 4, rgba, w * 4) != 0;
}

void tickPlayback(rigkit::MEcs& ecs, float dt) {
	const auto e = findSheet(ecs);
	if (e == entt::null) {
		return;
	}
	auto& s = ecs.getComponent<CSpriteSheet>(e);
	s = clampSpec(s);
	if (!s.playing || s.fps <= 0.f) {
		return;
	}
	static float accum = 0.f;
	accum += dt;
	const float step = 1.f / s.fps;
	while (accum >= step) {
		accum -= step;
		++s.frame;
		if (s.frame >= s.frameCount()) {
			s.frame = s.loop ? 0 : s.frameCount() - 1;
			if (!s.loop) {
				s.playing = false;
			}
		}
	}
}

} // namespace pixel
