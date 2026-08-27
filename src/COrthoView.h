#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Locked orthographic face. Speaks a view camera, not a solid.
 * @details Pose is derived (`rig::poseOrthoView`). Optics stay on `CCamera`
 * (`active` false so the window never steals the main bed).
 */
struct COrthoView {
	enum class Face : int { Top = 0, Bottom = 1, Left = 2, Right = 3 };

	Face face = Face::Top;
	glm::vec3 target{0.f, 0.f, 0.f};

	std::vector<sProp> GetProperties() {
		static constexpr const char* kFaces[] = {"Top", "Bottom", "Left", "Right"};
		return {{0, "Face", EPT_ENUM, reinterpret_cast<int*>(&face), kFaces, 4},
				{1, "Target X", EPT_FLOAT, &target.x},
				{2, "Target Y", EPT_FLOAT, &target.y},
				{3, "Target Z", EPT_FLOAT, &target.z}};
	}
};

} // namespace ecs
} // namespace rigkit
