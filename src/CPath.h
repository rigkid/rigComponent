#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace rigkit {
namespace ecs {

/**
 * @brief Editable vector stroke as plain commands. Speaks `rig.geometry.path`.
 * @details Portable POD — no toolkit/GPU types. Nested `Command` matches the
 * schema command object (`type` / `p` / `c1` / `c2`). Plot layer bags stay on
 * `CPaths` (rigPlotComponent); flatten / G-code emit live in code packs.
 * Command list is edited by path tools, not the generic property grid.
 */
struct CPath {
	static constexpr bool kNoProperties = true;

	enum class Cmd : uint8_t { MoveTo, LineTo, CubicTo, QuadTo, Close };

	/** @brief One path command — nested type of `rig.geometry.path`. */
	struct Command {
		Cmd type = Cmd::MoveTo;
		glm::vec2 p{0.f, 0.f};	///< MoveTo / LineTo endpoint; Close unused
		glm::vec2 c1{0.f, 0.f}; ///< Cubic/Quad control 1
		glm::vec2 c2{0.f, 0.f}; ///< Cubic control 2
	};

	std::vector<Command> commands;

	bool empty() const { return commands.empty(); }
};

} // namespace ecs
} // namespace rigkit
