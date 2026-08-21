#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include "CArc.h"
#include "CCadBoolean.h"
#include "CCadBox.h"
#include "CCadDimension.h"
#include "CCadCylinder.h"
#include "CCadSphere.h"
#include "CCamera.h"
#include "CDrawStyle.h"
#include "COrbitDrive.h"
#include "CEllipse.h"
#include "CGroup.h"
#include "CIndexedAtlas.h"
#include "CLight.h"
#include "CLine.h"
#include "CPalette.h"
#include "CMesh.h"
#include "CPolygon.h"
#include "CRectangle.h"
#include "CRegularPolygon.h"
#include "CRing.h"
#include "CSphere.h"
#include "CStar.h"
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
 * @brief Equilateral n-gon entity. @param x Centre x. @param y Centre y.
 * @param radius Centre to vertex. @param rotation Degrees from +X.
 */
entt::entity makeRegularPolygon(float x, float y, float radius, int sides, float rotation = 0.0f,
								const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
								const std::string& name = {});

entt::entity makeRegularPolygon(rigkit::MEcs& ecs, float x, float y, float radius, int sides,
								float rotation = 0.0f,
								const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
								const std::string& name = {});

/**
 * @brief Star entity. @param x Centre x. @param y Centre y.
 * @param radius Centre to outer point. @param innerRadius Absolute, not a ratio.
 */
entt::entity makeStar(float x, float y, float radius, float innerRadius, int points = 5,
					  float rotation = 0.0f,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

entt::entity makeStar(rigkit::MEcs& ecs, float x, float y, float radius, float innerRadius,
					  int points = 5, float rotation = 0.0f,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

/** @brief Freeform outline from a vertex list (needs three or more points). */
entt::entity makePolygon(std::vector<glm::vec2> points, bool closed = true,
						 const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						 const std::string& name = {});

entt::entity makePolygon(rigkit::MEcs& ecs, std::vector<glm::vec2> points, bool closed = true,
						 const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						 const std::string& name = {});

/** @brief Circular arc. @param pie Close through the centre. Angles in degrees. */
entt::entity makeArc(float x, float y, float radius, float startAngle, float endAngle,
					 bool pie = false, const rigkit::ecs::CDrawStyle& style = stroke(1, 1, 1),
					 const std::string& name = {});

entt::entity makeArc(rigkit::MEcs& ecs, float x, float y, float radius, float startAngle,
					 float endAngle, bool pie = false,
					 const rigkit::ecs::CDrawStyle& style = stroke(1, 1, 1),
					 const std::string& name = {});

/** @brief Annulus with a real hole, not a stroked ellipse. */
entt::entity makeRing(float x, float y, float outerRadius, float innerRadius,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
					  const std::string& name = {});

entt::entity makeRing(rigkit::MEcs& ecs, float x, float y, float outerRadius, float innerRadius,
					  const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
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
 * @brief XZ line grid (construction plane). Skipped by triangle pick.
 * @param half Cells from origin to each edge. @param y Plane height.
 */
entt::entity makeMeshGrid(int half = 6, float y = 0.f,
						  const rigkit::ecs::CDrawStyle& style = fill(0.32f, 0.34f, 0.38f),
						  const std::string& name = {});

entt::entity makeMeshGrid(rigkit::MEcs& ecs, int half = 6, float y = 0.f,
						  const rigkit::ecs::CDrawStyle& style = fill(0.32f, 0.34f, 0.38f),
						  const std::string& name = {});

/**
 * @brief Icosphere entity (`CSphere`), radius / subdivisions stay live data.
 * @details Present rebuilds triangles from `CSphere` each frame — same parametric
 * deal as `makeRegularPolygon` / `makeStar`, just in 3D. Want per-face colors or a
 * one-off hand-authored form instead? Build a `CMesh` directly (see `makeMesh`).
 */
entt::entity makeSphere(float radius = 1.0f, int subdivisions = 1,
						const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
						const std::string& name = {});

entt::entity makeSphere(rigkit::MEcs& ecs, float radius = 1.0f, int subdivisions = 1,
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
 * @brief Active camera with `COrbitDrive`, posed at yaw 0 looking at `target`.
 * @details Writes `CTransform` + `CCamera` + `COrbitDrive`. Auto-orbit starts
 * off (`enabled` false); set it true for a show-mode spin. `SOrbitDrive` yaws
 * when enabled. Mouse orbit / pan / dolly: `rig::orbitNavigate` in rigSystems.
 * @param pitch Radians; ~0.36 is a slight downward look.
 */
entt::entity makeOrbitCamera(glm::vec3 target = {0.f, 0.f, 0.f}, float radius = 5.6f,
							 float pitch = 0.36f, bool active = true,
							 const std::string& name = {});

entt::entity makeOrbitCamera(rigkit::MEcs& ecs, glm::vec3 target = {0.f, 0.f, 0.f},
							 float radius = 5.6f, float pitch = 0.36f, bool active = true,
							 const std::string& name = {});

/**
 * @brief CAD box solid (`CCadBox`). No draw style — bake or add `CDrawStyle`.
 */
entt::entity makeCadBox(float sizeX, float sizeY, float sizeZ, bool center = true,
						const std::string& name = {});

entt::entity makeCadBox(rigkit::MEcs& ecs, float sizeX, float sizeY, float sizeZ,
						bool center = true, const std::string& name = {});

/** @brief CAD cylinder (`CCadCylinder`), height along +Y. */
entt::entity makeCadCylinder(float radius, float height, bool center = true,
							 const std::string& name = {});

entt::entity makeCadCylinder(rigkit::MEcs& ecs, float radius, float height, bool center = true,
							 const std::string& name = {});

/** @brief CAD sphere (`CCadSphere`) at the local origin. */
entt::entity makeCadSphere(float radius, const std::string& name = {});

entt::entity makeCadSphere(rigkit::MEcs& ecs, float radius, const std::string& name = {});

/**
 * @brief CAD boolean (`CCadBoolean`). Operand strings are entity names.
 * @details Difference is `operands[0]` minus the rest. Adds `CDrawStyle`.
 */
entt::entity makeCadBoolean(rigkit::ecs::CCadBoolean::Op op, std::vector<std::string> operands,
							const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
							const std::string& name = {});

entt::entity makeCadBoolean(rigkit::MEcs& ecs, rigkit::ecs::CCadBoolean::Op op,
							std::vector<std::string> operands,
							const rigkit::ecs::CDrawStyle& style = fill(1, 1, 1),
							const std::string& name = {});

/**
 * @brief CAD datum (`CCadDimension`). `a` / `b` are entity names.
 * @details `measurement` true = label only. Pose for the label is optional.
 */
entt::entity makeCadDimension(rigkit::ecs::CCadDimension::Kind kind, const std::string& a,
							  const std::string& b, float value, bool measurement = false,
							  const std::string& name = {});

entt::entity makeCadDimension(rigkit::MEcs& ecs, rigkit::ecs::CCadDimension::Kind kind,
							  const std::string& a, const std::string& b, float value,
							  bool measurement = false, const std::string& name = {});

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
