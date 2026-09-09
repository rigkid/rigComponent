#pragma once

#include <memory>
#include <string>

#include "core/U_core.h"
#include "core/util/UndoStack.h"
#include "packs/rigProject/src/rigProject.h"
#include "PixelDoc.h"

class PixelApp : public rigkit::IApp {
  public:
	PixelApp() { settings().appName = "pixel"; }

	void setup() override;
	void update(float dt) override;
	void draw() override {}

	void drawNewDialog();

  private:
	void setupUi();
	void openNewDialog();
	void applyNewSheet();
	void openDocument();
	void saveDocument(bool saveAs);
	void exportSheet(bool bmp);
	void handleShortcuts();
	void pushLayerAdd(entt::entity e);
	void pushLayerDelete();

	std::shared_ptr<rigkit::rigProject> m_document;
	rigkit::UndoStack m_undo;
	pixel::ToolState m_tool;
	bool m_newDialog = false;
	CSpriteSheet m_newSpec;
	std::string m_docPath;
	bool m_ensureAfterLoad = false;
	bool m_ensureArmed = false;
};
