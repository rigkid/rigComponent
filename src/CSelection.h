#pragma once

namespace rigkit {
namespace ecs {

/**
 * @brief Runtime selection flags - written by pick / multi-select, not the property grid.
 */
struct CSelection {
	static constexpr bool kNoProperties = true;

	bool isSelected = false;
	bool isMultiSelected = false;
	int selectionIndex = -1;
};

} // namespace ecs
} // namespace rigkit
