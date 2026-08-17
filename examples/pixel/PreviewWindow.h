#pragma once

#include "IWindow.h"

class PreviewWindow : public rigkit::IWindow {
  public:
	PreviewWindow();

  protected:
	void renderContents() override;
};
