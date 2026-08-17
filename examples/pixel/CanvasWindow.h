#pragma once

#include <functional>
#include <string>
#include <vector>

#include "core/util/View2D.h"
#include "IWindow.h"
#include "PixelDoc.h"

class CanvasWindow : public rigkit::IWindow {
  public:
	CanvasWindow();

	void setTool(pixel::ToolState* tool) { m_tool = tool; }
	/** @brief Run during ImGui frame (e.g. File New modal). */
	void setFrameUi(std::function<void()> fn) { m_frameUi = std::move(fn); }

  protected:
	void renderContents() override;

  private:
	void paintAt(float contentX, float contentY);
	void beginStroke();
	void endStroke();

	pixel::ToolState* m_tool = nullptr;
	std::function<void()> m_frameUi;
	rigkit::View2D m_view;
	bool m_panning = false;
	ImVec2 m_lastPan{};
	bool m_stroke = false;
	entt::entity m_strokeEntity = entt::null;
	std::string m_strokeName;
	int m_strokeW = 0;
	int m_strokeH = 0;
	std::vector<uint8_t> m_strokeBefore;
};
