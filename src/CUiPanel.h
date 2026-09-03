#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/** @brief Portable tool surface. Speaks `rig.ui.panel`. */
struct CUiPanel {
	std::string role;
	int order = 0;
	bool visible = true;
	float preferredWidth = 320.f;
	float preferredHeight = 240.f;

	std::vector<sProp> GetProperties() {
		return {{0, "Role", EPT_STRING, &role},
				{1, "Order", EPT_INT, &order},
				{2, "Visible", EPT_BOOL, &visible},
				{3, "Pref Width", EPT_FLOAT, &preferredWidth},
				{4, "Pref Height", EPT_FLOAT, &preferredHeight}};
	}
};

/** @brief Section inside a panel. Speaks `rig.ui.group`. */
struct CUiGroup {
	uint32_t panel = 0;
	uint32_t parent = 0;
	int order = 0;
	std::string orientation = "vertical";
	bool collapsed = false;

	std::vector<sProp> GetProperties() {
		return {{0, "Order", EPT_INT, &order},
				{1, "Orientation", EPT_STRING, &orientation},
				{2, "Collapsed", EPT_BOOL, &collapsed}};
	}
};

/** @brief Control bound to an ECS property path. Speaks `rig.ui.control`. */
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
	std::string widget = "auto";

	std::vector<sProp> GetProperties() {
		return {{0, "Order", EPT_INT, &order},
				{1, "Property", EPT_STRING, &propertyKey},
				{2, "Type", EPT_STRING, &type},
				{3, "Widget", EPT_STRING, &widget},
				{4, "Enabled", EPT_BOOL, &enabled}};
	}
};

} // namespace ecs
} // namespace rigkit
