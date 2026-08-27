#include "PreviewWindow.h"

#include "core/RigKitEngine.h"
#include "ecs/MEcs.h"
#include "CSpriteSheet.h"
#include "PixelDoc.h"

#include <algorithm>
#include <imgui.h>
#include <vector>

PreviewWindow::PreviewWindow() : IWindow("Preview") {
	setCategory("Edit");
}

void PreviewWindow::renderContents() {
	auto* engine = getEngine();
	if (!engine || !engine->getECSManager()) {
		ImGui::TextDisabled("No ECS");
		return;
	}
	auto* ecs = engine->getECSManager();
	const auto sheetE = pixel::findSheet(*ecs);
	if (sheetE == entt::null) {
		ImGui::TextDisabled("No sprite sheet");
		return;
	}
	auto& spec = ecs->getComponent<CSpriteSheet>(sheetE);
	spec.frame = spec.frameCount() > 0 ? spec.frame % spec.frameCount() : 0;

	if (ImGui::Button(spec.playing ? "Pause" : "Play")) {
		spec.playing = !spec.playing;
	}
	ImGui::SameLine();
	if (ImGui::Button("Prev") && spec.frameCount() > 0) {
		spec.frame = (spec.frame + spec.frameCount() - 1) % spec.frameCount();
	}
	ImGui::SameLine();
	if (ImGui::Button("Next") && spec.frameCount() > 0) {
		spec.frame = (spec.frame + 1) % spec.frameCount();
	}
	ImGui::SliderFloat("FPS", &spec.fps, 1.f, 24.f, "%.0f");
	ImGui::Checkbox("Loop", &spec.loop);
	ImGui::Text("Frame %d / %d", spec.frame + 1, spec.frameCount());

	std::vector<uint8_t> rgba;
	int w = 0, h = 0;
	pixel::compositeFrame(*ecs, spec.frame, rgba, w, h);
	if (w <= 0 || h <= 0 || rgba.empty()) {
		return;
	}

	const ImVec2 avail = ImGui::GetContentRegionAvail();
	const float scale = (std::max)(1.f, std::floor((std::min)(avail.x / static_cast<float>(w),
															  avail.y / static_cast<float>(h))));
	const ImVec2 origin = ImGui::GetCursorScreenPos();
	ImDrawList* dl = ImGui::GetWindowDrawList();
	const ImVec2 end{origin.x + static_cast<float>(w) * scale,
					 origin.y + static_cast<float>(h) * scale};
	const float cell = (std::max)(4.f, scale);
	for (float y = origin.y; y < end.y; y += cell) {
		for (float x = origin.x; x < end.x; x += cell) {
			const int cx = static_cast<int>((x - origin.x) / cell);
			const int cy = static_cast<int>((y - origin.y) / cell);
			const ImU32 c = ((cx + cy) & 1) ? IM_COL32(48, 48, 54, 255) : IM_COL32(36, 36, 40, 255);
			dl->AddRectFilled(ImVec2(x, y),
							  ImVec2((std::min)(x + cell, end.x), (std::min)(y + cell, end.y)), c);
		}
	}
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			const size_t i = static_cast<size_t>((y * w + x) * 4);
			if (rgba[i + 3] == 0) {
				continue;
			}
			dl->AddRectFilled(ImVec2(origin.x + static_cast<float>(x) * scale,
									 origin.y + static_cast<float>(y) * scale),
							  ImVec2(origin.x + static_cast<float>(x + 1) * scale,
									 origin.y + static_cast<float>(y + 1) * scale),
							  IM_COL32(rgba[i], rgba[i + 1], rgba[i + 2], rgba[i + 3]));
		}
	}
	ImGui::Dummy(ImVec2(static_cast<float>(w) * scale, static_cast<float>(h) * scale));
}
