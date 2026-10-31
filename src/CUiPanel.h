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

/** @brief Portable tool or HUD surface. Speaks `rig.ui.panel`. */
struct CUiPanel {
	enum class Surface : int { Tool = 0, Hud = 1 };

	std::string role;
	Surface surface = Surface::Tool;
	int order = 0;
	bool visible = true;
	float preferredWidth = 320.f;
	float preferredHeight = 240.f;

	static constexpr const char* kSurfaceNames[] = {"tool", "hud"};

	std::vector<sProp> GetProperties() {
		return {{0, "Role", EPT_STRING, &role},
				{1, "Surface", EPT_ENUM, reinterpret_cast<int*>(&surface), kSurfaceNames, 2},
				{2, "Order", EPT_INT, &order},
				{3, "Visible", EPT_BOOL, &visible},
				{4, "Pref Width", EPT_FLOAT, &preferredWidth},
				{5, "Pref Height", EPT_FLOAT, &preferredHeight}};
	}
};

/** @brief Section inside a panel. Speaks `rig.ui.group`. */
struct CUiGroup {
	enum class Orientation : int { Vertical = 0, Horizontal = 1 };

	uint32_t panel = 0;
	uint32_t parent = 0;
	int order = 0;
	Orientation orientation = Orientation::Vertical;
	bool collapsed = false;

	static constexpr const char* kOrientationNames[] = {"vertical", "horizontal"};

	std::vector<sProp> GetProperties() {
		return {{0, "Panel", EPT_UINT, &panel},
				{1, "Parent", EPT_UINT, &parent},
				{2, "Order", EPT_INT, &order},
				{3, "Orientation", EPT_ENUM, reinterpret_cast<int*>(&orientation),
				 kOrientationNames, 2},
				{4, "Collapsed", EPT_BOOL, &collapsed}};
	}
};

/** @brief View over one POD field. Speaks `rig.ui.control`. */
struct CUiControl {
	enum class Widget : int {
		Auto = 0,
		Slider,
		Knob,
		Toggle,
		Field,
		Dropdown,
		Color,
		Xy
	};

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
	Widget widget = Widget::Auto;

	static constexpr const char* kWidgetNames[] = {"auto",	 "slider", "knob",  "toggle",
												   "field", "dropdown", "color", "xy"};

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
				{11, "Widget", EPT_ENUM, reinterpret_cast<int*>(&widget), kWidgetNames, 8}};
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

inline const char* uiSurfaceToken(CUiPanel::Surface surface) {
	return surface == CUiPanel::Surface::Hud ? "hud" : "tool";
}

inline CUiPanel::Surface uiSurfaceFromToken(const std::string& token) {
	return token == "hud" ? CUiPanel::Surface::Hud : CUiPanel::Surface::Tool;
}

inline const char* uiOrientToken(CUiGroup::Orientation orientation) {
	return orientation == CUiGroup::Orientation::Horizontal ? "horizontal" : "vertical";
}

inline CUiGroup::Orientation uiOrientFromToken(const std::string& token) {
	return token == "horizontal" ? CUiGroup::Orientation::Horizontal
								 : CUiGroup::Orientation::Vertical;
}

inline const char* uiWidgetToken(CUiControl::Widget widget) {
	const int i = static_cast<int>(widget);
	if (i >= static_cast<int>(CUiControl::Widget::Auto) &&
		i <= static_cast<int>(CUiControl::Widget::Xy)) {
		return CUiControl::kWidgetNames[i];
	}
	return "auto";
}

inline CUiControl::Widget uiWidgetFromToken(const std::string& token) {
	for (int i = static_cast<int>(CUiControl::Widget::Auto);
		 i <= static_cast<int>(CUiControl::Widget::Xy); ++i) {
		if (token == CUiControl::kWidgetNames[i]) {
			return static_cast<CUiControl::Widget>(i);
		}
	}
	return CUiControl::Widget::Auto;
}

} // namespace ecs
} // namespace rigkit
