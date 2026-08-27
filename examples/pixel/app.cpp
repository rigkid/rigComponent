#include "app.h"

#include "core/pack/MPack.h"
#include "core/RigKitEngine.h"
#include "core/util/AppPaths.h"
#include "rendering/U_gladGlfw.h"
#include "packs/rigComponent/src/rigComponent.h"
#include "packs/rigImGui/src/ImGuiStyleKit.h"
#include "packs/rigImGui/src/Mui.h"
#include "packs/rigImGui/src/MWindow.h"
#include "packs/rigImGui/src/rigImGui.h"
#include "packs/rigSystems/src/rigSystems.h"
#include "BrushesWindow.h"
#include "CanvasWindow.h"
#include "CProject.h"
#include "PreviewWindow.h"

#include <algorithm>
#include <cstdio>
#include <imgui.h>
#include <imgui_internal.h>
#include <memory>
#include <spdlog/spdlog.h>

void PixelApp::setup() {
	spdlog::info("pixel — sprite-sheet editor");
	m_engine->setClearColor(0.10f, 0.10f, 0.12f, 1.0f);

	auto* packs = m_engine->getPackManager();
	if (!packs) {
		return;
	}
	packs->registerPack<rigkit::rigComponent>();
	packs->registerPack<rigkit::rigSystems>();
	packs->registerPack<rigkit::rigProject>();
	packs->registerPack<rigkit::rigImGui>();
	packs->initAll();
	packs->setupAll();

	m_document = packs->getPack<rigkit::rigProject>();
	auto* ecs = m_engine->getECSManager();
	if (!ecs || !m_document) {
		return;
	}

	ecs->registerComponent<CSpriteSheet>("SpriteSheet", true);
	pixel::registerSerializers(*m_document);

	m_docPath = m_document->documentPath(AppPaths::joinPath(AppPaths::getDataDir(), "sprite"));
	m_newSpec = CSpriteSheet{};
	pixel::newDocument(*ecs, m_newSpec, true);
	if (const auto proj = pixel::findProject(*ecs); proj != entt::null) {
		ecs->getComponent<rigkit::ecs::CProject>(proj).path = m_docPath;
	}

	setupUi();
}

void PixelApp::setupUi() {
	auto* ui = m_engine->getUiManager();
	auto* mui = dynamic_cast<rigkit::Mui*>(ui);
	if (!mui) {
		return;
	}

	mui->setImGuiTheme(rigkit::ImGuiTheme::Dark);
	mui->uiPrefs().showStatusBar = true;
	mui->setUndoStack(&m_undo);
	mui->setAboutIntro("pixel — indexed sprite-sheet editor.\n"
					   "Pencil / eraser / fill, layers, PNG/BMP export, frame playback.\n"
					   "Index 0 is transparent. File New sets frame size and sheet columns/rows.");

	mui->addHostPanel(rigkit::HostPanel::Scene);
	mui->addHostPanel(rigkit::HostPanel::Layers);
	mui->addHostPanel(rigkit::HostPanel::Properties);
	mui->addHostPanel(rigkit::HostPanel::Log);

	auto* wm = ui->getWindowManager();
	if (wm) {
		if (auto canvas = wm->createWindow<CanvasWindow>()) {
			canvas->setTool(&m_tool);
			canvas->setFrameUi([this] { drawNewDialog(); });
			canvas->setVisible(true);
		}
		if (auto brushes = wm->createWindow<BrushesWindow>()) {
			brushes->setTool(&m_tool);
			brushes->setVisible(true);
		}
		if (auto preview = wm->createWindow<PreviewWindow>()) {
			preview->setVisible(true);
		}
	}

	mui->registerFileAction("New...", [this] { openNewDialog(); }, "Ctrl+N");
	mui->registerFileAction("Open...", [this] { openDocument(); }, "Ctrl+O");
	mui->registerFileAction("Save", [this] { saveDocument(false); }, "Ctrl+S");
	mui->registerFileAction("Save As...", [this] { saveDocument(true); });
	mui->setRecentFileOpenHandler([this](const std::string& path) {
		if (m_document) {
			m_document->requestLoad(path);
			m_docPath = path;
			m_ensureAfterLoad = true;
			m_ensureArmed = false;
			m_undo.clear();
		}
	});
	mui->registerExportAction("PNG (sheet)...", [this] { exportSheet(false); });
	mui->registerExportAction("BMP (sheet)...", [this] { exportSheet(true); });
	mui->registerAppAction("New Layer", [this] {
		if (auto* ecs = m_engine->getECSManager()) {
			char name[32];
			std::snprintf(name, sizeof(name), "Layer %d",
						  static_cast<int>(pixel::layersByOrder(*ecs).size()) + 1);
			pushLayerAdd(pixel::addLayer(*ecs, name));
		}
	});
	mui->registerAppAction("Delete Layer", [this] { pushLayerDelete(); });

	mui->statusBar().setSlot({"tool",
							  [this] {
								  switch (m_tool.tool) {
								  case pixel::Tool::Eraser:
									  return std::string("Eraser");
								  case pixel::Tool::Fill:
									  return std::string("Fill");
								  case pixel::Tool::Pencil:
								  default:
									  return std::string("Pencil");
								  }
							  },
							  80.f});
	mui->statusBar().setSlot(
		{"hint", [] { return std::string("B pencil  E eraser  G fill  P play  [ ] size"); }, 0.f});

	mui->setDockLayoutBuilder([mui](ImGuiID dockspaceId) {
		ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspaceId);
		if (!node) {
			return;
		}
		if (node->IsSplitNode()) {
			mui->setDockLayoutBuilder(nullptr);
			return;
		}

		const ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspaceId);
		ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceId, vp->WorkSize);

		ImGuiID left = 0, center = 0, right = 0, bottom = 0, leftTop = 0;
		ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.18f, &left, &center);
		ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.24f, &right, &center);
		ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.28f, &bottom, &center);
		ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.45f, &left, &leftTop);

		ImGui::DockBuilderDockWindow("Brushes", leftTop);
		ImGui::DockBuilderDockWindow("Layers", left);
		ImGui::DockBuilderDockWindow("Scene", left);
		ImGui::DockBuilderDockWindow("Canvas", center);
		ImGui::DockBuilderDockWindow("Properties", right);
		ImGui::DockBuilderDockWindow("Preview", bottom);
		ImGui::DockBuilderDockWindow("Log", bottom);
		ImGui::DockBuilderFinish(dockspaceId);
		mui->setDockLayoutBuilder(nullptr);
	});
}

void PixelApp::openNewDialog() {
	if (auto* ecs = m_engine->getECSManager()) {
		if (const auto e = pixel::findSheet(*ecs); e != entt::null) {
			m_newSpec = ecs->getComponent<CSpriteSheet>(e);
		}
	}
	m_newDialog = true;
}

void PixelApp::drawNewDialog() {
	if (m_newDialog) {
		ImGui::OpenPopup("New Sprite Sheet");
		m_newDialog = false;
	}
	if (!ImGui::BeginPopupModal("New Sprite Sheet", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		return;
	}
	ImGui::InputInt("Frame width", &m_newSpec.tileWidth);
	ImGui::InputInt("Frame height", &m_newSpec.tileHeight);
	ImGui::InputInt("Columns", &m_newSpec.tilesAcross);
	ImGui::InputInt("Rows", &m_newSpec.tileRows);
	m_newSpec.tileWidth = (std::max)(1, m_newSpec.tileWidth);
	m_newSpec.tileHeight = (std::max)(1, m_newSpec.tileHeight);
	m_newSpec.tilesAcross = (std::max)(1, m_newSpec.tilesAcross);
	m_newSpec.tileRows = (std::max)(1, m_newSpec.tileRows);
	ImGui::Text("Sheet %d x %d  (%d frames)", m_newSpec.sheetWidth(), m_newSpec.sheetHeight(),
				m_newSpec.frameCount());
	ImGui::TextDisabled("Index 0 is transparent. Max sheet 256 x 256.");
	if (ImGui::Button("Create", ImVec2(120.f, 0.f))) {
		applyNewSheet();
		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(120.f, 0.f))) {
		ImGui::CloseCurrentPopup();
	}
	ImGui::EndPopup();
}

void PixelApp::applyNewSheet() {
	auto* ecs = m_engine->getECSManager();
	if (!ecs) {
		return;
	}
	pixel::newDocument(*ecs, m_newSpec, false);
	m_undo.clear();
	if (const auto proj = pixel::findProject(*ecs); proj != entt::null) {
		ecs->getComponent<rigkit::ecs::CProject>(proj).path = m_docPath;
	}
	if (auto* mui = dynamic_cast<rigkit::Mui*>(m_engine->getUiManager())) {
		mui->showNotification("New sprite sheet");
	}
}

void PixelApp::openDocument() {
	auto* ui = m_engine->getUiManager();
	if (!ui) {
		return;
	}
	ui->openFileDialog("Open Sprite Sheet", {".rig"}, [this](const std::string& path) {
		if (!m_document || path.empty()) {
			return;
		}
		m_document->requestLoad(path);
		m_docPath = path;
		m_ensureAfterLoad = true;
		m_ensureArmed = false;
		m_undo.clear();
		if (auto* mui = dynamic_cast<rigkit::Mui*>(m_engine->getUiManager())) {
			mui->noteRecentFile(path);
		}
	});
}

void PixelApp::saveDocument(bool saveAs) {
	auto* ecs = m_engine->getECSManager();
	if (!ecs || !m_document) {
		return;
	}
	auto doSave = [this](const std::string& path) {
		if (!m_document || path.empty()) {
			return;
		}
		m_docPath = path;
		if (auto* e = m_engine->getECSManager()) {
			if (const auto proj = pixel::findProject(*e); proj != entt::null) {
				e->getComponent<rigkit::ecs::CProject>(proj).path = path;
			}
		}
		m_document->requestSave(path);
		if (auto* mui = dynamic_cast<rigkit::Mui*>(m_engine->getUiManager())) {
			mui->noteRecentFile(path);
			mui->showNotification("Saved " + path);
		}
	};
	if (!saveAs && !m_docPath.empty()) {
		doSave(m_docPath);
		return;
	}
	auto* ui = m_engine->getUiManager();
	if (!ui) {
		return;
	}
	ui->saveFileDialog("Save Sprite Sheet", {".rig"}, doSave);
}

void PixelApp::exportSheet(bool bmp) {
	auto* ui = m_engine->getUiManager();
	auto* ecs = m_engine->getECSManager();
	if (!ui || !ecs) {
		return;
	}
	ui->saveFileDialog(bmp ? "Export BMP" : "Export PNG", {bmp ? ".bmp" : ".png"},
					   [this, bmp](const std::string& path) {
						   auto* e = m_engine->getECSManager();
						   if (!e || path.empty()) {
							   return;
						   }
						   std::vector<uint8_t> rgba;
						   int w = 0, h = 0;
						   pixel::compositeSheet(*e, rgba, w, h);
						   auto* mui = dynamic_cast<rigkit::Mui*>(m_engine->getUiManager());
						   if (pixel::exportImage(path, rgba.data(), w, h)) {
							   if (mui) {
								   mui->showNotification(
									   std::string(bmp ? "Exported BMP " : "Exported PNG ") + path);
							   }
						   } else if (mui) {
							   mui->showNotification("Export failed",
													 rigkit::NotificationType::Error);
						   }
					   });
}

void PixelApp::pushLayerAdd(entt::entity e) {
	auto* ecs = m_engine ? m_engine->getECSManager() : nullptr;
	if (!ecs || e == entt::null) {
		return;
	}
	pixel::LayerSnap snap;
	if (!pixel::captureLayer(*ecs, e, snap)) {
		return;
	}
	auto held = std::make_shared<entt::entity>(e);
	m_undo.push(
		"Add Layer",
		[this, held] {
			if (auto* ecs = m_engine ? m_engine->getECSManager() : nullptr) {
				pixel::destroyLayer(*ecs, *held);
				*held = entt::null;
			}
		},
		[this, held, snap] {
			if (auto* ecs = m_engine ? m_engine->getECSManager() : nullptr) {
				*held = pixel::restoreLayer(*ecs, snap);
			}
		});
}

void PixelApp::pushLayerDelete() {
	auto* ecs = m_engine ? m_engine->getECSManager() : nullptr;
	if (!ecs) {
		return;
	}
	const auto e = pixel::selectedLayer(*ecs);
	if (e == entt::null || pixel::layersByOrder(*ecs).size() <= 1) {
		return;
	}
	pixel::LayerSnap snap;
	if (!pixel::captureLayer(*ecs, e, snap)) {
		return;
	}
	auto held = std::make_shared<entt::entity>(entt::null);
	pixel::destroyLayer(*ecs, e);
	m_undo.push(
		"Delete Layer",
		[this, held, snap] {
			if (auto* ecs = m_engine ? m_engine->getECSManager() : nullptr) {
				*held = pixel::restoreLayer(*ecs, snap);
			}
		},
		[this, held] {
			if (auto* ecs = m_engine ? m_engine->getECSManager() : nullptr) {
				pixel::destroyLayer(*ecs, *held);
				*held = entt::null;
			}
		});
}

void PixelApp::handleShortcuts() {
	GLFWwindow* win = m_engine ? m_engine->getWindow() : nullptr;
	if (!win) {
		return;
	}
	const bool uiBlocks = ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureKeyboard;

	static bool wasB = false, wasE = false, wasG = false, wasP = false;
	static bool wasLb = false, wasRb = false;
	const bool b = glfwGetKey(win, GLFW_KEY_B) == GLFW_PRESS;
	const bool e = glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS;
	const bool g = glfwGetKey(win, GLFW_KEY_G) == GLFW_PRESS;
	const bool p = glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS;
	const bool lb = glfwGetKey(win, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS;
	const bool rb = glfwGetKey(win, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS;

	if (!uiBlocks) {
		if (b && !wasB) {
			m_tool.tool = pixel::Tool::Pencil;
		}
		if (e && !wasE) {
			m_tool.tool = pixel::Tool::Eraser;
		}
		if (g && !wasG) {
			m_tool.tool = pixel::Tool::Fill;
		}
		if (p && !wasP) {
			if (auto* ecs = m_engine->getECSManager()) {
				if (const auto sheet = pixel::findSheet(*ecs); sheet != entt::null) {
					auto& s = ecs->getComponent<CSpriteSheet>(sheet);
					s.playing = !s.playing;
				}
			}
		}
		if (lb && !wasLb) {
			m_tool.size = (std::max)(1, m_tool.size - 1);
		}
		if (rb && !wasRb) {
			m_tool.size = (std::min)(8, m_tool.size + 1);
		}
	}
	wasB = b;
	wasE = e;
	wasG = g;
	wasP = p;
	wasLb = lb;
	wasRb = rb;
}

void PixelApp::update(float dt) {
	// Load runs in ecs updateSystems after this — arm one frame, then ensure.
	if (m_ensureAfterLoad) {
		if (m_ensureArmed) {
			if (auto* ecs = m_engine->getECSManager()) {
				pixel::ensureAfterLoad(*ecs);
			}
			m_ensureAfterLoad = false;
			m_ensureArmed = false;
		} else {
			m_ensureArmed = true;
		}
	}
	if (auto* ecs = m_engine->getECSManager()) {
		pixel::tickPlayback(*ecs, dt);
	}
	handleShortcuts();
}
