#pragma once

/**
 * @file
 * @brief Portable tool / HUD surfaces: `CUiPanel`, `CUiGroup`, `CUiControl`, `CUiAction`.
 * @details Speaks @c rig\.ui\.panel / group / control / action. Labels compose
 * `rig.meta.named`. Present is **rigImGui** `presentUiPanels`.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

inline constexpr int kUiSurfaceTool = 0;
inline constexpr int kUiSurfaceHud = 1;
inline constexpr int kUiOrientVertical = 0;
inline constexpr int kUiOrientHorizontal = 1;
inline constexpr int kUiWidgetAuto = 0;
inline constexpr int kUiWidgetSlider = 1;
inline constexpr int kUiWidgetKnob = 2;
inline constexpr int kUiWidgetToggle = 3;
inline constexpr int kUiWidgetField = 4;
inline constexpr int kUiWidgetDropdown = 5;
inline constexpr int kUiWidgetColor = 6;
inline constexpr int kUiWidgetXy = 7;

inline constexpr const char* kUiSurfaceNames[] = {"tool", "hud"};
inline constexpr const char* kUiOrientNames[] = {"vertical", "horizontal"};
inline constexpr const char* kUiWidgetNames[] = {"auto",	 "slider", "knob",  "toggle",
												 "field", "dropdown", "color", "xy"};

inline const char* uiSurfaceToken(int surface) {
	return surface == kUiSurfaceHud ? "hud" : "tool";
}

inline int uiSurfaceFromToken(const std::string& token) {
	return token == "hud" ? kUiSurfaceHud : kUiSurfaceTool;
}

inline const char* uiOrientToken(int orientation) {
	return orientation == kUiOrientHorizontal ? "horizontal" : "vertical";
}

inline int uiOrientFromToken(const std::string& token) {
	return token == "horizontal" ? kUiOrientHorizontal : kUiOrientVertical;
}

inline const char* uiWidgetToken(int widget) {
	if (widget >= kUiWidgetAuto && widget <= kUiWidgetXy) {
		return kUiWidgetNames[widget];
	}
	return "auto";
}

inline int uiWidgetFromToken(const std::string& token) {
	for (int i = kUiWidgetAuto; i <= kUiWidgetXy; ++i) {
		if (token == kUiWidgetNames[i]) {
			return i;
		}
	}
	return kUiWidgetAuto;
}

/** @brief Portable tool or HUD surface. Speaks `rig.ui.panel`. */
struct CUiPanel {
	std::string role;
	int surface = kUiSurfaceTool;
	int order = 0;
	bool visible = true;
	float preferredWidth = 320.f;
	float preferredHeight = 240.f;

	std::vector<sProp> GetProperties() {
		return {{0, "Role", EPT_STRING, &role},
				{1, "Surface", EPT_ENUM, &surface, kUiSurfaceNames, 2},
				{2, "Order", EPT_INT, &order},
				{3, "Visible", EPT_BOOL, &visible},
				{4, "Pref Width", EPT_FLOAT, &preferredWidth},
				{5, "Pref Height", EPT_FLOAT, &preferredHeight}};
	}
};

/** @brief Section inside a panel. Speaks `rig.ui.group`. */
struct CUiGroup {
	uint32_t panel = 0;
	uint32_t parent = 0;
	int order = 0;
	int orientation = kUiOrientVertical;
	bool collapsed = false;

	std::vector<sProp> GetProperties() {
		return {{0, "Panel", EPT_UINT, &panel},
				{1, "Parent", EPT_UINT, &parent},
				{2, "Order", EPT_INT, &order},
				{3, "Orientation", EPT_ENUM, &orientation, kUiOrientNames, 2},
				{4, "Collapsed", EPT_BOOL, &collapsed}};
	}
};

/** @brief View over one POD field. Speaks `rig.ui.control`. */
struct CUiControl {
	uint32_t panel = 0;
	uint32_t group = 0;
	int order = 0;
	uint32_t target = 0;
	std::string propertyKey;
	std::string type = "float";
	float min = 0.f;
	float max = 1.f;
	float step = 0.01f;
	bool enabled = true;
	bool readOnly = false;
	std::vector<std::string> options;
	int widget = kUiWidgetAuto;

	std::vector<sProp> GetProperties() {
		return {{0, "Panel", EPT_UINT, &panel},
				{1, "Group", EPT_UINT, &group},
				{2, "Order", EPT_INT, &order},
				{3, "Target", EPT_UINT, &target},
				{4, "Property", EPT_STRING, &propertyKey},
				{5, "Type", EPT_STRING, &type},
				{6, "Min", EPT_FLOAT, &min},
				{7, "Max", EPT_FLOAT, &max},
				{8, "Step", EPT_FLOAT, &step},
				{9, "Enabled", EPT_BOOL, &enabled},
				{10, "Read Only", EPT_BOOL, &readOnly},
				{11, "Widget", EPT_ENUM, &widget, kUiWidgetNames, 8}};
	}
};

/** @brief Command button on a panel. Speaks `rig.ui.action`. */
struct CUiAction {
	uint32_t panel = 0;
	uint32_t group = 0;
	int order = 0;
	std::string actionId;
	bool enabled = true;

	std::vector<sProp> GetProperties() {
		return {{0, "Panel", EPT_UINT, &panel},
				{1, "Group", EPT_UINT, &group},
				{2, "Order", EPT_INT, &order},
				{3, "Action", EPT_STRING, &actionId},
				{4, "Enabled", EPT_BOOL, &enabled}};
	}
};

} // namespace ecs
} // namespace rigkit
