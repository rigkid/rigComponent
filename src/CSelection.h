#pragma once

namespace rigkit {
namespace ecs {

struct CSelection {
	bool isSelected = false;
	bool isMultiSelected = false;
	int selectionIndex = -1;
};

} // namespace ecs
} // namespace rigkit
