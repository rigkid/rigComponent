#pragma once

#include "IWindow.h"
#include "PixelDoc.h"

class BrushesWindow : public rigkit::IWindow {
  public:
	BrushesWindow();

	void setTool(pixel::ToolState* tool) { m_tool = tool; }

  protected:
	void renderContents() override;

  private:
	pixel::ToolState* m_tool = nullptr;
};
