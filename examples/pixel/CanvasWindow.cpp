#include "CanvasWindow.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include <imgui.h>

#include "core/IMui.h"
#include "core/RigKitEngine.h"
#include "core/util/UndoStack.h"
#include "ecs/MEcs.h"
#include "CIndexedAtlas.h"
#include "CSpriteSheet.h"

namespace {
ImVec2 toIm(const glm::vec2& v) {
	return {v.x, v.y};
}
} // namespace

CanvasWindow::CanvasWindow() : IWindow("Canvas") {
	setCategory("Edit");
	m_view.zoomMin = 0.25f;
	m_view.zoomMax = 32.f;
	m_view.zoom = 4.f;
}

void CanvasWindow::paintAt(float contentX, float contentY) {
	if (!m_tool || !getEngine() || !getEngine()->getECSManager()) {
		return;
	}
	const int x = static_cast<int>(std::floor(contentX));
	const int y = static_cast<int>(std::floor(contentY));
	pixel::paint(*getEngine()->getECSManager(), x, y, *m_tool);
}

void CanvasWindow::beginStroke() {
	m_stroke = true;
	m_strokeEntity = entt::null;
	m_strokeName.clear();
	m_strokeBefore.clear();
	m_strokeW = 0;
	m_strokeH = 0;
	auto* ecs = getEngine() ? getEngine()->getECSManager() : nullptr;
	if (!ecs) {
		return;
	}
	const auto layer = pixel::selectedLayer(*ecs);
	if (layer == entt::null || !ecs->hasComponent<rigkit::ecs::CIndexedAtlas>(layer)) {
		return;
	}
	const auto& atlas = ecs->getComponent<rigkit::ecs::CIndexedAtlas>(layer);
	m_strokeEntity = layer;
	m_strokeName = ecs->entityName(layer);
	m_strokeW = atlas.width;
	m_strokeH = atlas.height;
	m_strokeBefore = atlas.indices;
}

void CanvasWindow::endStroke() {
	if (!m_stroke) {
		return;
	}
	m_stroke = false;
	auto* engine = getEngine();
	auto* ecs = engine ? engine->getECSManager() : nullptr;
	auto* ui = engine ? engine->getUiManager() : nullptr;
	auto* stack = ui ? ui->undoStack() : nullptr;
	if (!ecs || !stack || m_strokeEntity == entt::null ||
		!ecs->hasComponent<rigkit::ecs::CIndexedAtlas>(m_strokeEntity)) {
		m_strokeBefore.clear();
		return;
	}
	const auto& atlas = ecs->getComponent<rigkit::ecs::CIndexedAtlas>(m_strokeEntity);
	if (atlas.indices == m_strokeBefore) {
		m_strokeBefore.clear();
		return;
	}
	const char* label = "Paint";
	if (m_tool) {
		switch (m_tool->tool) {
		case pixel::Tool::Eraser:
			label = "Eraser";
			break;
		case pixel::Tool::Fill:
			label = "Fill";
			break;
		case pixel::Tool::Pencil:
		default:
			label = "Pencil";
			break;
		}
	}
	const auto entity = m_strokeEntity;
	const auto name = m_strokeName;
	const int w = m_strokeW;
	const int h = m_strokeH;
	stack->pushSnapshot(label, std::move(m_strokeBefore), atlas.indices,
						[engine, entity, name, w, h](const std::vector<uint8_t>& indices) {
							if (!engine || !engine->getECSManager()) {
								return;
							}
							pixel::applyAtlas(*engine->getECSManager(), entity, name, w, h,
											  indices);
						});
}

void CanvasWindow::renderContents() {
	if (m_frameUi) {
		m_frameUi();
	}

	auto* engine = getEngine();
	if (!engine || !engine->getECSManager()) {
		ImGui::TextDisabled("No ECS");
		return;
	}
	auto* ecs = engine->getECSManager();

	if (ImGui::Button("Fit")) {
		m_view.fitToCanvas();
	}
	ImGui::SameLine();
	ImGui::TextDisabled("Wheel zoom  MMB pan  LMB paint");

	std::vector<uint8_t> rgba;
	int sw = 0, sh = 0;
	pixel::compositeSheet(*ecs, rgba, sw, sh);
	if (sw <= 0 || sh <= 0) {
		ImGui::TextDisabled("No sheet");
		return;
	}
	m_view.contentSize = {static_cast<float>(sw), static_cast<float>(sh)};

	ImVec2 avail = ImGui::GetContentRegionAvail();
	avail.x = (std::max)(avail.x, 1.f);
	avail.y = (std::max)(avail.y, 1.f);
	const ImVec2 canvasPos = ImGui::GetCursorScreenPos();
	ImGui::InvisibleButton("##pixel_canvas", avail);
	const bool hovered = ImGui::IsItemHovered();
	m_view.hovered = hovered;
	m_view.canvasOrigin = {canvasPos.x, canvasPos.y};
	m_view.canvasW = avail.x;
	m_view.canvasH = avail.y;
	m_view.updateDerived();

	ImDrawList* dl = ImGui::GetWindowDrawList();
	dl->AddRectFilled(canvasPos, ImVec2(canvasPos.x + avail.x, canvasPos.y + avail.y),
					  IM_COL32(22, 22, 26, 255));
	dl->PushClipRect(canvasPos, ImVec2(canvasPos.x + avail.x, canvasPos.y + avail.y), true);

	const float z = m_view.zoomAbs;
	const ImVec2 sheet0{m_view.ox, m_view.oy};
	const ImVec2 sheet1{m_view.ox + static_cast<float>(sw) * z,
						m_view.oy + static_cast<float>(sh) * z};

	// Checkerboard under transparent index 0.
	const float cell = (std::max)(4.f, z);
	for (float y = sheet0.y; y < sheet1.y; y += cell) {
		for (float x = sheet0.x; x < sheet1.x; x += cell) {
			const int cx = static_cast<int>((x - sheet0.x) / cell);
			const int cy = static_cast<int>((y - sheet0.y) / cell);
			const ImU32 c = ((cx + cy) & 1) ? IM_COL32(48, 48, 54, 255) : IM_COL32(36, 36, 40, 255);
			dl->AddRectFilled(
				ImVec2(x, y),
				ImVec2((std::min)(x + cell, sheet1.x), (std::min)(y + cell, sheet1.y)), c);
		}
	}

	const int x0 = (std::max)(0, static_cast<int>(std::floor((canvasPos.x - m_view.ox) / z)));
	const int y0 = (std::max)(0, static_cast<int>(std::floor((canvasPos.y - m_view.oy) / z)));
	const int x1 =
		(std::min)(sw, static_cast<int>(std::ceil((canvasPos.x + avail.x - m_view.ox) / z)));
	const int y1 =
		(std::min)(sh, static_cast<int>(std::ceil((canvasPos.y + avail.y - m_view.oy) / z)));
	int step = 1;
	const int vis = (std::max)(1, (x1 - x0) * (y1 - y0));
	if (vis > 16000) {
		step = static_cast<int>(std::ceil(std::sqrt(vis / 16000.0)));
	}
	for (int y = y0; y < y1; y += step) {
		for (int x = x0; x < x1; x += step) {
			const size_t i = static_cast<size_t>((y * sw + x) * 4);
			if (i + 3 >= rgba.size() || rgba[i + 3] == 0) {
				continue;
			}
			const ImVec2 a = toIm(m_view.toScreen(static_cast<float>(x), static_cast<float>(y)));
			const ImVec2 b =
				toIm(m_view.toScreen(static_cast<float>(x + step), static_cast<float>(y + step)));
			dl->AddRectFilled(ImVec2(a.x, a.y), ImVec2(b.x, b.y),
							  IM_COL32(rgba[i], rgba[i + 1], rgba[i + 2], rgba[i + 3]));
		}
	}

	CSpriteSheet spec;
	if (const auto sheet = pixel::findSheet(*ecs); sheet != entt::null) {
		spec = ecs->getComponent<CSpriteSheet>(sheet);
	}
	if (z >= 2.f) {
		for (int x = 0; x <= sw; ++x) {
			const ImVec2 a = toIm(m_view.toScreen(static_cast<float>(x), 0.f));
			const ImVec2 b = toIm(m_view.toScreen(static_cast<float>(x), static_cast<float>(sh)));
			dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(70, 70, 78, 90));
		}
		for (int y = 0; y <= sh; ++y) {
			const ImVec2 a = toIm(m_view.toScreen(0.f, static_cast<float>(y)));
			const ImVec2 b = toIm(m_view.toScreen(static_cast<float>(sw), static_cast<float>(y)));
			dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(70, 70, 78, 90));
		}
	}
	for (int c = 0; c <= spec.tilesAcross; ++c) {
		const float x = static_cast<float>(c * spec.tileWidth);
		const ImVec2 a = toIm(m_view.toScreen(x, 0.f));
		const ImVec2 b = toIm(m_view.toScreen(x, static_cast<float>(sh)));
		dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(220, 180, 70, 180));
	}
	for (int r = 0; r <= spec.tileRows; ++r) {
		const float y = static_cast<float>(r * spec.tileHeight);
		const ImVec2 a = toIm(m_view.toScreen(0.f, y));
		const ImVec2 b = toIm(m_view.toScreen(static_cast<float>(sw), y));
		dl->AddLine(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(220, 180, 70, 180));
	}
	int fx = 0, fy = 0;
	spec.frameOrigin(spec.frame, fx, fy);
	{
		const ImVec2 a = toIm(m_view.toScreen(static_cast<float>(fx), static_cast<float>(fy)));
		const ImVec2 b = toIm(m_view.toScreen(static_cast<float>(fx + spec.tileWidth),
											 static_cast<float>(fy + spec.tileHeight)));
		dl->AddRect(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(80, 200, 255, 220), 0.f, 0, 2.f);
	}

	if (hovered) {
		const ImVec2 mouse = ImGui::GetIO().MousePos;
		const glm::vec2 content = m_view.toContent(mouse.x, mouse.y);
		const int px = static_cast<int>(std::floor(content.x));
		const int py = static_cast<int>(std::floor(content.y));
		if (px >= 0 && py >= 0 && px < sw && py < sh) {
			const ImVec2 a = toIm(m_view.toScreen(static_cast<float>(px), static_cast<float>(py)));
			const ImVec2 b =
				toIm(m_view.toScreen(static_cast<float>(px + 1), static_cast<float>(py + 1)));
			dl->AddRect(ImVec2(a.x, a.y), ImVec2(b.x, b.y), IM_COL32(255, 255, 255, 200));
		}
	}
	dl->PopClipRect();

	if (hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		if (!m_panning) {
			m_panning = true;
			m_lastPan = ImGui::GetIO().MousePos;
		} else {
			const ImVec2 now = ImGui::GetIO().MousePos;
			m_view.applyPanDelta(now.x - m_lastPan.x, now.y - m_lastPan.y);
			m_lastPan = now;
		}
	} else {
		m_panning = false;
	}
	if (hovered) {
		const float wheel = ImGui::GetIO().MouseWheel;
		if (std::abs(wheel) > 1e-4f) {
			const ImVec2 mouse = ImGui::GetIO().MousePos;
			m_view.applyScrollZoom(wheel, mouse.x, mouse.y);
		}
	}
	if (hovered && !m_panning && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		if (!m_stroke) {
			beginStroke();
		}
		const ImVec2 mouse = ImGui::GetIO().MousePos;
		const glm::vec2 content = m_view.toContent(mouse.x, mouse.y);
		paintAt(content.x, content.y);
	} else if (m_stroke) {
		endStroke();
	}
}
