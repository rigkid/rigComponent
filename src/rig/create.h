#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include "CCamera.h"
#include "CDrawStyle.h"
#include "CGroup.h"
#include "CIndexedAtlas.h"
#include "CLight.h"
#include "CPalette.h"
#include "CMesh.h"
#include "CShape.h"
#include "CTransform.h"
#include "ecs/MEcs.h"

namespace rig {

/**
 * @brief Entity creators — write POD components only.
 * @details NO CODE JUST DATA. Present via rigSystems (2D) or rigRender3D (active camera).
 *
 * Every creator comes in two forms. The bare form writes to the ECS bound by
 * setEcs, which registering **rigComponent** already does, so a sketch reads:
 *
 * ```cpp
 * rig::makeRect(40.f, 40.f, 220.f, 170.f, rig::fill(0.2f, 0.56f, 0.96f));
 * ```
 *
 * The form taking an explicit `MEcs&` stays for packs, tools, and tests that
 * target a specific registry.
 */

inline rigkit::ecs::CDrawStyle fill(float r, float g, float b, float a = 1.0f) {
	rigkit::ecs::CDrawStyle s;
	s.setFillColor(r, g, b, a);
	s.hasStroke = false;
	return s;
}

inline rigkit::ecs::CDrawStyle stroke(float r, float g, float b, float a = 1.0f,
									  float width = 1.0f) {
	rigkit::ecs::CDrawStyle s;
	s.hasFill = false;
	s.hasStroke = true;
	s.setStrokeColor(r, g, b, a);
	s.strokeWidth = width;
	return s;
}

inline rigkit::ecs::CDrawStyle fillAndStroke(float fr, float fg, float fb, float sr, float sg,
											 float sb, float strokeWidth = 1.0f) {
	rigkit::ecs::CDrawStyle s;
	s.setFillColor(fr, fg, fb, 1.0f);
	s.setStrokeColor(sr, sg, sb, 1.0f);
	s.strokeWidth = strokeWidth;
	s.hasFill = true;
	s.hasStroke = true;
	return s;
}

/**
 * @brief Bind the ECS that the bare creators write to.
 * @details `rigComponent::setup()` binds the host ECS, so apps rarely call this.
 * Mirrors `rig::setGraphics` for immediate draw. Pass nullptr to unbind.
 * @see currentEcs
 */
void setEcs(rigkit::MEcs* ecs);

/** @brief ECS the bare creators write to, or nullptr when nothing is bound. */
rigkit::MEcs* currentEcs();

/**
 * @brief Create a rectangle entity (transform + shape + draw style).
 * @param x Top-left x. @param y Top-left y. @param w Width. @param h Height.
 * @return New entity id, or `entt::null` when no ECS is bound.
 */
entt::entity makeRect(float x, float y, float w, float h,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

entt::entity makeRect(rigkit::MEcs& ecs, float x, float y, float w, float h,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

/** @param x Centre x. @param y Centre y. */
entt::entity makeEllipse(float x, float y, float w, float h,
						 const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						 const std::string& name = {});

entt::entity makeEllipse(rigkit::MEcs& ecs, float x, float y, float w, float h,
						 const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						 const std::string& name = {});

/** @param x Centre x. @param y Centre y. */
entt::entity makeCircle(float x, float y, float radius,
						const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						const std::string& name = {});

entt::entity makeCircle(rigkit::MEcs& ecs, float x, float y, float radius,
						const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						const std::string& name = {});

entt::entity makeLine(float x1, float y1, float x2, float y2,
					  const rigkit::ecs::CDrawStyle& style = stroke(1, 1, 1),
					  const std::string& name = {});

entt::entity makeLine(rigkit::MEcs& ecs, float x1, float y1, float x2, float y2,
					  const rigkit::ecs::CDrawStyle& style = stroke(1, 1, 1),
					  const std::string& name = {});

/**
 * @brief Mesh entity from positions (+ optional indices).
 * @details Present path draws triangles or lines per CMesh::Mode.
 * Triangle winding is caller-owned: CCW when viewed along the intended
 * outward normal (same rule as `makeMeshBox` / `makeMeshQuad`).
 */
entt::entity makeMesh(std::vector<glm::vec3> positions, std::vector<uint32_t> indices = {},
					  rigkit::ecs::CMesh::Mode mode = rigkit::ecs::CMesh::Mode::Triangles,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

entt::entity makeMesh(rigkit::MEcs& ecs, std::vector<glm::vec3> positions,
					  std::vector<uint32_t> indices = {},
					  rigkit::ecs::CMesh::Mode mode = rigkit::ecs::CMesh::Mode::Triangles,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

/** @brief Triangle `a,b,c` in that index order (CCW => outward / +facing normal). */
entt::entity makeMeshTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
							  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
							  const std::string& name = {});

entt::entity makeMeshTriangle(rigkit::MEcs& ecs, glm::vec3 a, glm::vec3 b, glm::vec3 c,
							  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
							  const std::string& name = {});

/** @brief Axis-aligned quad on z = 0 (CCW in XY => +Z normal). */
entt::entity makeMeshQuad(float x, float y, float w, float h,
						  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						  const std::string& name = {});

entt::entity makeMeshQuad(rigkit::MEcs& ecs, float x, float y, float w, float h,
						  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						  const std::string& name = {});

/** @brief Axis-aligned box at origin (size = full extents). Faces CCW from outside. */
entt::entity makeMeshBox(float size = 1.0f,
						 const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						 const std::string& name = {});

entt::entity makeMeshBox(rigkit::MEcs& ecs, float size = 1.0f,
						 const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						 const std::string& name = {});

/**
 * @brief Aim a transform at a world point (writes position + rotation quat).
 * @details Builds camera-to-world from `inverse(lookAt)` so `CCamera::viewMatrix`
 * (inverse world) matches the usual OpenGL eye. Syncs euler cache.
 */
inline void lookAt(rigkit::ecs::CTransform& xf, const glm::vec3& eye, const glm::vec3& target,
				   const glm::vec3& up = {0.f, 1.f, 0.f}) {
	xf.position = eye;
	const glm::mat4 view = glm::lookAt(eye, target, up);
	const glm::mat4 world = glm::inverse(view);
	xf.setRotationQuat(glm::normalize(glm::quat_cast(glm::mat3(world))));
}

/**
 * @brief Camera entity (`CTransform` + `CCamera`).
 * @details Present via `rigRender3D` when `CCamera::active`. Pose is transform.
 */
entt::entity makeCamera(glm::vec3 position, bool active = true, const std::string& name = {});

entt::entity makeCamera(rigkit::MEcs& ecs, glm::vec3 position, bool active = true,
						const std::string& name = {});

/**
 * @brief Light entity (`CTransform` + `CLight`).
 * @details Directional: shines along local -Z. Point: emits from transform position.
 */
entt::entity makeLight(glm::vec3 position,
					   rigkit::ecs::CLight::Type type = rigkit::ecs::CLight::Type::Directional,
					   const std::string& name = {});

entt::entity makeLight(rigkit::MEcs& ecs, glm::vec3 position,
					   rigkit::ecs::CLight::Type type = rigkit::ecs::CLight::Type::Directional,
					   const std::string& name = {});

/** @brief Scene palette entity (`CPalette`). Default = `CPalette::default16()`. */
entt::entity makePalette(rigkit::ecs::CPalette palette = rigkit::ecs::CPalette::default16(),
						 const std::string& name = {});

entt::entity makePalette(rigkit::MEcs& ecs,
						 rigkit::ecs::CPalette palette = rigkit::ecs::CPalette::default16(),
						 const std::string& name = {});

/** @brief Indexed atlas entity (`CIndexedAtlas`). Default 128x128 filled with index 0. */
entt::entity makeIndexedAtlas(int width = 128, int height = 128, uint8_t fillIndex = 0,
							  const std::string& name = {});

entt::entity makeIndexedAtlas(rigkit::MEcs& ecs, int width = 128, int height = 128,
							  uint8_t fillIndex = 0, const std::string& name = {});

/** @brief Scene group root (`CTransform` + `CGroup`). Children use `CRelationship::parent`. */
entt::entity makeGroup(glm::vec3 position = {0, 0, 0}, const std::string& name = {});

entt::entity makeGroup(rigkit::MEcs& ecs, glm::vec3 position = {0, 0, 0},
					   const std::string& name = {});

/**
 * @brief Parent one entity to another (`CRelationship`).
 * @details Child keeps its local TRS; `SHierarchy` folds parent world before Draw.
 */
void parentTo(entt::entity child, entt::entity parent);

void parentTo(rigkit::MEcs& ecs, entt::entity child, entt::entity parent);

/**
 * @brief Mark an entity selected (`CSelection`).
 * @details The present path strokes a bounds overlay around selected entities.
 */
void select(entt::entity entity, bool selected = true);

void select(rigkit::MEcs& ecs, entt::entity entity, bool selected = true);

} // namespace rig
