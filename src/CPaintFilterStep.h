#pragma once

#include <cstdint>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace rigkit {
namespace ecs {

/**
 * @brief One filter primitive. Speaks `rig.paint.filter_step`.
 */
struct CPaintFilterStep {
	static constexpr bool kNoProperties = true;

	enum class Kind : uint8_t {
		Blend,
		ColorMatrix,
		ComponentTransfer,
		Composite,
		Convolve,
		DiffuseLight,
		Displacement,
		DropShadow,
		Flood,
		Blur,
		Image,
		Merge,
		Morphology,
		Offset,
		SpecularLight,
		Tile,
		Turbulence
	};
	enum class Operator : uint8_t { Over, In, Out, Atop, Xor, Arithmetic, Erode, Dilate };

	Kind kind = Kind::Blur;
	std::string in;
	std::string in2;
	float stdDeviation = 0.f;
	glm::vec2 offset{0.f, 0.f};
	std::vector<float> values;
	Operator op = Operator::Over;
	float radius = 0.f;
	glm::vec4 rgba{0.f, 0.f, 0.f, 1.f};
	entt::entity asset{entt::null};
};

} // namespace ecs
} // namespace rigkit
