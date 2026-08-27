#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "ecs/PropertyReflection.h"
#include "CTransform.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Portable camera POD - optics plus present chrome.
 * @details Pose lives on `CTransform` (same entity). Speaks `rig.spatial.camera`.
 * `rigRender3D` builds view from `CTransform::world` and projection from these fields.
 * `presentWidth` / `presentHeight` / `showFps` / `shade` are present choices, not optics.
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
	/// Corner FPS readout (`SFpsOverlay` in rigRender3D). Off by default.
	bool showFps = false;
	/// Present shade. Wireframe is see-through edges; Sketch hides occluded edges.
	enum class Shade : int { Solid = 0, Wireframe = 1, Sketch = 2 };
	Shade shade = Shade::Solid;

	/// Restore optics defaults (FOV, clips, projection). Keeps Active and
	/// present chrome (lo-res size, FPS overlay, shade are product choices).
	void resetToDefaults() {
		const bool keepActive = active;
		const int keepW = presentWidth;
		const int keepH = presentHeight;
		const bool keepFps = showFps;
		const Shade keepShade = shade;
		*this = CCamera{};
		active = keepActive;
		presentWidth = keepW;
		presentHeight = keepH;
		showFps = keepFps;
		shade = keepShade;
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

	/**
	 * @brief Project a world point into viewport pixels (origin top-left).
	 * @return false when the point is behind the camera (`clip.w` near zero).
	 */
	static bool projectToViewport(const CCamera& cam, const CTransform& camXf, glm::vec3 world,
								  float viewW, float viewH, glm::vec2& outPx) {
		if (viewW <= 1.f || viewH <= 1.f) {
			return false;
		}
		const glm::mat4 vp = cam.projectionMatrix(viewW / viewH) * viewMatrix(camXf);
		const glm::vec4 clip = vp * glm::vec4(world, 1.f);
		if (std::fabs(clip.w) < 1e-6f) {
			return false;
		}
		const glm::vec3 ndc = glm::vec3(clip) / clip.w;
		outPx.x = (ndc.x * 0.5f + 0.5f) * viewW;
		outPx.y = (1.f - (ndc.y * 0.5f + 0.5f)) * viewH;
		return true;
	}

	std::vector<sProp> GetProperties() {
		static constexpr const char* kProjectionNames[] = {"Perspective", "Orthographic"};
		static constexpr const char* kShadeNames[] = {"Solid", "Wireframe", "Sketch"};
		return {
			{0, "Active", EPT_BOOL, &active},
			{1, "Projection", EPT_ENUM, reinterpret_cast<int*>(&projection), kProjectionNames, 2},
			{2, "FOV Y (deg)", EPT_FLOAT, &fovYDegrees},
			{3, "Ortho Height", EPT_FLOAT, &orthoHeight},
			{4, "Near", EPT_FLOAT, &nearClip},
			{5, "Far", EPT_FLOAT, &farClip},
			{6, "Aspect (0=viewport)", EPT_FLOAT, &aspect},
			{7, "Present Width (0=full)", EPT_INT, &presentWidth},
			{8, "Present Height (0=full)", EPT_INT, &presentHeight},
			{9, "Show FPS", EPT_BOOL, &showFps},
			{10, "Shade", EPT_ENUM, reinterpret_cast<int*>(&shade), kShadeNames, 3}};
	}
};

} // namespace ecs
} // namespace rigkit
