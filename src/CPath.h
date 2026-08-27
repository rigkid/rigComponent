#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace rigkit {
namespace ecs {

/**
 * @brief Editable vector stroke as plain commands. Speaks `rig.geometry.path`.
 * @details Portable POD — no toolkit/GPU types. Nested `Command` matches the
 * schema command object (`type` / `point` / `control1` / `control2`). ArcTo is
 * a host command for SVG A; `.rig` path serialize expands it to cubics. Plot
 * layer bags stay on `CPaths` (rigPlotComponent); flatten / G-code emit live in
 * code packs. Command list is edited by path tools, not the generic property grid.
 */
struct CPath {
	static constexpr bool kNoProperties = true;

	enum class Cmd : uint8_t { MoveTo, LineTo, CubicTo, QuadTo, Close, ArcTo };

	/** @brief One path command — nested type of `rig.geometry.path`. */
	struct Command {
		Cmd type = Cmd::MoveTo;
		glm::vec2 p{0.f, 0.f};	   ///< MoveTo / LineTo / cubic/quad/arc endpoint; Close unused
		glm::vec2 c1{0.f, 0.f};	   ///< Cubic/Quad control1
		glm::vec2 c2{0.f, 0.f};	   ///< Cubic control2
		glm::vec2 radii{0.f, 0.f}; ///< ArcTo rx/ry (host; SVG A)
		float xRotDeg = 0.f;	   ///< ArcTo x-axis rotation, degrees
		bool largeArc = false;	   ///< ArcTo large-arc flag
		bool sweep = false;		   ///< ArcTo sweep flag
	};

	std::vector<Command> commands;

	bool empty() const { return commands.empty(); }
};

} // namespace ecs
} // namespace rigkit
