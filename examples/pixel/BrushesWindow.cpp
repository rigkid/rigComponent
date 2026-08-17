#include "BrushesWindow.h"

#include <cstdio>

#include <imgui.h>

#include "core/RigKitEngine.h"
#include "ecs/MEcs.h"
#include "CPalette.h"

BrushesWindow::BrushesWindow() : IWindow("Brushes") {
	setCategory("Edit");
}

void BrushesWindow::renderContents() {
	if (!m_tool) {
		ImGui::TextDisabled("No tool state");
		return;
	}

	auto toolButton = [&](const char* id, const char* label, pixel::Tool tool) {
		const bool active = m_tool->tool == tool;
		if (active) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		}
		if (ImGui::Button(label, ImVec2(-1.f, 0.f))) {
			m_tool->tool = tool;
		}
		if (active) {
			ImGui::PopStyleColor();
		}
		(void)id;
	};

	toolButton("pencil", "Pencil", pixel::Tool::Pencil);
	toolButton("eraser", "Eraser", pixel::Tool::Eraser);
	toolButton("fill", "Fill", pixel::Tool::Fill);

	ImGui::Separator();
	ImGui::TextUnformatted("Shape");
	if (ImGui::RadioButton("Square", m_tool->shape == pixel::BrushShape::Square)) {
		m_tool->shape = pixel::BrushShape::Square;
	}
	if (ImGui::RadioButton("Circle", m_tool->shape == pixel::BrushShape::Circle)) {
		m_tool->shape = pixel::BrushShape::Circle;
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Size");
	ImGui::SliderInt("##size", &m_tool->size, 1, 8, "%d px");
	const int presets[] = {1, 2, 4, 8};
	for (int i = 0; i < 4; ++i) {
		if (i > 0) {
			ImGui::SameLine();
		}
		char buf[8];
		std::snprintf(buf, sizeof(buf), "%d", presets[i]);
		if (ImGui::SmallButton(buf)) {
			m_tool->size = presets[i];
		}
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Palette");
	rigkit::ecs::CPalette palette = rigkit::ecs::CPalette::default16();
	if (auto* engine = getEngine()) {
		if (auto* ecs = engine->getECSManager()) {
			if (const auto e = pixel::findPalette(*ecs); e != entt::null) {
				palette = ecs->getComponent<rigkit::ecs::CPalette>(e);
			}
		}
	}
	const float sw = ImGui::GetContentRegionAvail().x;
	const float cell = (std::max)(18.f, sw / 4.f - 4.f);
	for (int i = 0; i < rigkit::ecs::CPalette::kCount; ++i) {
		if (i % 4 != 0) {
			ImGui::SameLine();
		}
		const auto c = palette.color(i);
		ImGui::PushID(i);
		if (m_tool->color == i) {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 1.f, 1.f, 1.f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
		}
		if (ImGui::ColorButton("##sw", ImVec4(c.r, c.g, c.b, c.a),
							   ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
							   ImVec2(cell, cell))) {
			m_tool->color = i;
			if (m_tool->tool == pixel::Tool::Eraser) {
				m_tool->tool = pixel::Tool::Pencil;
			}
		}
		if (m_tool->color == i) {
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip(i == 0 ? "Index 0 — transparent" : "Index %d", i);
		}
		ImGui::PopID();
	}
}
