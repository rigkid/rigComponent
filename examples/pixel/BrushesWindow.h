#pragma once

#include "ImGuiPanel.h"
#include "PixelDoc.h"

class BrushesWindow : public rigkit::ImGuiPanel {
  public:
	BrushesWindow();

	void setTool(pixel::ToolState* tool) { m_tool = tool; }

  protected:
	void renderContents() override;

  private:
	pixel::ToolState* m_tool = nullptr;
};
