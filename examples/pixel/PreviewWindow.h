#pragma once

#include "ImGuiPanel.h"
class PreviewWindow : public rigkit::ImGuiPanel {
  public:
	PreviewWindow();

  protected:
	void renderContents() override;
};
