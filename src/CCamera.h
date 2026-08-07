#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "CTransform.h"
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Portable camera POD — projection params only.
 * @details Pose lives on `CTransform` (same entity). Speaks `rig.spatial.camera`.
 * `rigRender3D` builds view from `CTransform::world` and projection from these fields.
 * No GPU handles.
 */
struct CCamera {
	enum class Projection : int { Perspective = 0, Orthographic = 1 };

	bool active = true;
	Projection projection = Projection::Perspective;
	/// Vertical field of view in degrees (perspective).
	float fovYDegrees = 45.0f;
	/// Full vertical size in world units (orthographic).
	float orthoHeight = 10.0f;
	float nearClip = 0.1f;
	float farClip = 1000.0f;
	/// 0 = use viewport aspect at present time.
	float aspect = 0.0f;
	/// Low-res present size (0 = full window). Nearest-upscaled by `rigRender3D`.
	int presentWidth = 0;
	int presentHeight = 0;

	/// Restore optics defaults (FOV, clips, projection). Keeps Active and
	/// present size (lo-res present is a product choice, not an optic default).
	void resetToDefaults() {
		const bool keepActive = active;
		const int keepW = presentWidth;
		const int keepH = presentHeight;
		*this = CCamera{};
		active = keepActive;
		presentWidth = keepW;
		presentHeight = keepH;
	}

	glm::mat4 projectionMatrix(float viewportAspect) const {
		const float a = aspect > 0.0f ? aspect : (viewportAspect > 0.0f ? viewportAspect : 1.0f);
		if (projection == Projection::Orthographic) {
			const float halfH = orthoHeight * 0.5f;
			const float halfW = halfH * a;
			return glm::ortho(-halfW, halfW, -halfH, halfH, nearClip, farClip);
		}
		return glm::perspective(glm::radians(fovYDegrees), a, nearClip, farClip);
	}

	/// View from hierarchy world pose (call after `SHierarchy`).
	static glm::mat4 viewMatrix(const CTransform& transform) {
		return glm::inverse(transform.world);
	}

	std::vector<sProp> GetProperties() {
		static constexpr const char* kProjectionNames[] = {"Perspective", "Orthographic"};
		return {{0, "Active", EPT_BOOL, &active},
				{1, "Projection", EPT_ENUM, reinterpret_cast<int*>(&projection), kProjectionNames,
				 2},
				{2, "FOV Y (deg)", EPT_FLOAT, &fovYDegrees},
				{3, "Ortho Height", EPT_FLOAT, &orthoHeight},
				{4, "Near", EPT_FLOAT, &nearClip},
				{5, "Far", EPT_FLOAT, &farClip},
				{6, "Aspect (0=viewport)", EPT_FLOAT, &aspect},
				{7, "Present Width (0=full)", EPT_INT, &presentWidth},
				{8, "Present Height (0=full)", EPT_INT, &presentHeight}};
	}
};

} // namespace ecs
} // namespace rigkit
