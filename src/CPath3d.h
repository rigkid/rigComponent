#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace rigkit {
namespace ecs {

/**
 * @brief Editable 3-space stroke as plain commands. Speaks `rig.geometry.path3d`.
 * @details Sibling of `CPath` - same command grammar, `vec3` points. No `ArcTo`:
 * circular arcs stay on `rig.geometry.arc`; snippet G2/G3 already chord to
 * lines. Planar contours stay on `CPath`; do not fake 2D with z = 0 here.
 * Command list is edited by path tools, not the generic property grid.
 */
struct CPath3d {
	static constexpr bool kNoProperties = true;

	enum class Cmd : uint8_t { MoveTo, LineTo, CubicTo, QuadTo, Close };

	/** @brief One path command - nested type of `rig.geometry.path3d`. */
	struct Command {
		Cmd type = Cmd::MoveTo;
		glm::vec3 p{0.f};  ///< MoveTo / LineTo / cubic/quad endpoint; Close unused
		glm::vec3 c1{0.f}; ///< Cubic/Quad control1
		glm::vec3 c2{0.f}; ///< Cubic control2
	};

	std::vector<Command> commands;

	bool empty() const { return commands.empty(); }
};

} // namespace ecs
} // namespace rigkit
