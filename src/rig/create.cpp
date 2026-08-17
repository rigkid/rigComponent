#include "rig/create.h"
#include <spdlog/spdlog.h>
#include "CGroup.h"
#include "CRelationship.h"
#include "CSelectable.h"
#include "CSelection.h"
#include "MeshFaces.h"

namespace rig {
namespace {

rigkit::MEcs* g_ecs = nullptr;

/// Bound ECS, or nullptr after one complaint so a missing bootstrap is visible.
rigkit::MEcs* boundEcs() {
	static bool warned = false;
	if (!g_ecs && !warned) {
		warned = true;
		spdlog::error("rig: no ECS bound. Register rigComponent, or call rig::setEcs, "
					  "before creating entities.");
	}
	return g_ecs;
}

rigkit::ecs::CTransform at(float x, float y, float z = 0.0f) {
	rigkit::ecs::CTransform t;
	t.position = {x, y, z};
	return t;
}

/// Placement rides the transform, so every primitive stays local to its own origin.
template <typename TShape>
entt::entity spawnShape(rigkit::MEcs& ecs, TShape shape, const rigkit::ecs::CDrawStyle& style,
						const rigkit::ecs::CTransform& xf, const std::string& name) {
	auto e = ecs.createEntity(name);
	ecs.addComponent<rigkit::ecs::CTransform>(e, xf);
	ecs.addComponent<TShape>(e, std::move(shape));
	ecs.addComponent<rigkit::ecs::CDrawStyle>(e, style);
	ecs.addComponent<rigkit::ecs::CSelectable>(e, rigkit::ecs::CSelectable{});
	return e;
}

} // namespace

void setEcs(rigkit::MEcs* ecs) {
	g_ecs = ecs;
}

rigkit::MEcs* currentEcs() {
	return g_ecs;
}

entt::entity makeRect(rigkit::MEcs& ecs, float x, float y, float w, float h,
					  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	rigkit::ecs::CRectangle shape;
	shape.width = w;
	shape.height = h;
	return spawnShape(ecs, shape, style, at(x, y), name);
}

entt::entity makeEllipse(rigkit::MEcs& ecs, float x, float y, float w, float h,
						 const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	rigkit::ecs::CEllipse shape;
	shape.rx = w * 0.5f;
	shape.ry = h * 0.5f;
	return spawnShape(ecs, shape, style, at(x, y), name);
}

entt::entity makeCircle(rigkit::MEcs& ecs, float x, float y, float radius,
						const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	return makeEllipse(ecs, x, y, radius * 2.0f, radius * 2.0f, style, name);
}

entt::entity makeLine(rigkit::MEcs& ecs, float x1, float y1, float x2, float y2,
					  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	rigkit::ecs::CLine shape;
	shape.x1 = x1;
	shape.y1 = y1;
	shape.x2 = x2;
	shape.y2 = y2;
	return spawnShape(ecs, shape, style, at(0, 0), name);
}

entt::entity makeRegularPolygon(rigkit::MEcs& ecs, float x, float y, float radius, int sides,
								float rotation, const rigkit::ecs::CDrawStyle& style,
								const std::string& name) {
	rigkit::ecs::CRegularPolygon shape;
	shape.radius = radius;
	shape.sides = sides;
	shape.rotationDegrees = rotation;
	return spawnShape(ecs, shape, style, at(x, y), name);
}

entt::entity makeStar(rigkit::MEcs& ecs, float x, float y, float radius, float innerRadius,
					  int points, float rotation, const rigkit::ecs::CDrawStyle& style,
					  const std::string& name) {
	rigkit::ecs::CStar shape;
	shape.radius = radius;
	shape.innerRadius = innerRadius;
	shape.points = points;
	shape.rotationDegrees = rotation;
	return spawnShape(ecs, shape, style, at(x, y), name);
}

entt::entity makePolygon(rigkit::MEcs& ecs, std::vector<glm::vec2> points, bool closed,
						 const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	rigkit::ecs::CPolygon shape;
	shape.points = std::move(points);
	shape.closed = closed;
	return spawnShape(ecs, std::move(shape), style, at(0, 0), name);
}

entt::entity makeArc(rigkit::MEcs& ecs, float x, float y, float radius, float startAngle,
					 float endAngle, bool pie, const rigkit::ecs::CDrawStyle& style,
					 const std::string& name) {
	rigkit::ecs::CArc shape;
	shape.setRadius(radius);
	shape.startAngleDegrees = startAngle;
	shape.endAngleDegrees = endAngle;
	shape.pie = pie;
	return spawnShape(ecs, shape, style, at(x, y), name);
}

entt::entity makeRing(rigkit::MEcs& ecs, float x, float y, float outerRadius, float innerRadius,
					  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	rigkit::ecs::CRing shape;
	shape.outerRadius = outerRadius;
	shape.innerRadius = innerRadius;
	return spawnShape(ecs, shape, style, at(x, y), name);
}

entt::entity makeMesh(rigkit::MEcs& ecs, std::vector<glm::vec3> positions,
					  std::vector<uint32_t> indices, rigkit::ecs::CMesh::Mode mode,
					  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto e = ecs.createEntity(name);
	ecs.addComponent<rigkit::ecs::CTransform>(e, at(0, 0));
	rigkit::ecs::CMesh mesh;
	mesh.mode = mode;
	mesh.positions = std::move(positions);
	mesh.indices = std::move(indices);
	rigkit::ecs::meshFinishFaces(mesh);
	ecs.addComponent<rigkit::ecs::CMesh>(e, std::move(mesh));
	ecs.addComponent<rigkit::ecs::CDrawStyle>(e, style);
	return e;
}

entt::entity makeMeshTriangle(rigkit::MEcs& ecs, glm::vec3 a, glm::vec3 b, glm::vec3 c,
							  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	return makeMesh(ecs, {a, b, c}, {0, 1, 2}, rigkit::ecs::CMesh::Mode::Triangles, style, name);
}

entt::entity makeMeshQuad(rigkit::MEcs& ecs, float x, float y, float w, float h,
						  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	const glm::vec3 p0{x, y, 0.0f};
	const glm::vec3 p1{x + w, y, 0.0f};
	const glm::vec3 p2{x + w, y + h, 0.0f};
	const glm::vec3 p3{x, y + h, 0.0f};
	return makeMesh(ecs, {p0, p1, p2, p3}, {0, 1, 2, 0, 2, 3}, rigkit::ecs::CMesh::Mode::Triangles,
					style, name);
}

entt::entity makeMeshBox(rigkit::MEcs& ecs, float size, const rigkit::ecs::CDrawStyle& style,
						 const std::string& name) {
	const float h = size * 0.5f;
	const std::vector<glm::vec3> positions = {
		{-h, -h, -h}, {h, -h, -h}, {h, h, -h}, {-h, h, -h}, // back z-
		{-h, -h, h},  {h, -h, h},  {h, h, h},  {-h, h, h},	// front z+
	};
	// CCW from outside so face normals point outward (Lambert / rigRender3D).
	const std::vector<uint32_t> indices = {
		0, 2, 1, 0, 3, 2, // back  -Z
		4, 5, 6, 4, 6, 7, // front +Z
		0, 1, 5, 0, 5, 4, // bottom -Y
		2, 3, 7, 2, 7, 6, // top +Y
		0, 4, 7, 0, 7, 3, // left -X
		1, 2, 6, 1, 6, 5, // right +X
	};
	return makeMesh(ecs, positions, indices, rigkit::ecs::CMesh::Mode::Triangles, style, name);
}

entt::entity makeSphere(rigkit::MEcs& ecs, float radius, int subdivisions,
						const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	rigkit::ecs::CSphere shape;
	shape.radius = radius;
	shape.subdivisions = subdivisions;
	return spawnShape(ecs, shape, style, at(0, 0, 0), name);
}

entt::entity makeCamera(rigkit::MEcs& ecs, glm::vec3 position, bool active,
						const std::string& name) {
	auto e = ecs.createEntity(name);
	rigkit::ecs::CTransform xf;
	xf.position = position;
	ecs.addComponent<rigkit::ecs::CTransform>(e, xf);
	rigkit::ecs::CCamera cam;
	cam.active = active;
	ecs.addComponent<rigkit::ecs::CCamera>(e, cam);
	return e;
}

entt::entity makeLight(rigkit::MEcs& ecs, glm::vec3 position, rigkit::ecs::CLight::Type type,
					   const std::string& name) {
	auto e = ecs.createEntity(name);
	rigkit::ecs::CTransform xf;
	xf.position = position;
	ecs.addComponent<rigkit::ecs::CTransform>(e, xf);
	rigkit::ecs::CLight light;
	light.type = type;
	ecs.addComponent<rigkit::ecs::CLight>(e, light);
	return e;
}

entt::entity makePalette(rigkit::MEcs& ecs, rigkit::ecs::CPalette palette,
						 const std::string& name) {
	auto e = ecs.createEntity(name);
	ecs.addComponent<rigkit::ecs::CPalette>(e, std::move(palette));
	return e;
}

entt::entity makeIndexedAtlas(rigkit::MEcs& ecs, int width, int height, uint8_t fillIndex,
							  const std::string& name) {
	auto e = ecs.createEntity(name);
	rigkit::ecs::CIndexedAtlas atlas;
	atlas.resize(width, height, fillIndex);
	ecs.addComponent<rigkit::ecs::CIndexedAtlas>(e, std::move(atlas));
	return e;
}

entt::entity makeGroup(rigkit::MEcs& ecs, glm::vec3 position, const std::string& name) {
	auto e = ecs.createEntity(name.empty() ? "group" : name);
	rigkit::ecs::CTransform xf;
	xf.position = position;
	ecs.addComponent<rigkit::ecs::CTransform>(e, xf);
	ecs.addComponent<rigkit::ecs::CGroup>(e, rigkit::ecs::CGroup{});
	return e;
}

void parentTo(rigkit::MEcs& ecs, entt::entity child, entt::entity parent) {
	rigkit::ecs::CRelationship rel;
	rel.parent = parent;
	ecs.addComponent<rigkit::ecs::CRelationship>(child, rel);
}

void select(rigkit::MEcs& ecs, entt::entity entity, bool selected) {
	rigkit::ecs::CSelection sel;
	sel.isSelected = selected;
	ecs.addComponent<rigkit::ecs::CSelection>(entity, sel);
}

// Bare forms — same creators against the ECS bound by setEcs.

entt::entity makeRect(float x, float y, float w, float h, const rigkit::ecs::CDrawStyle& style,
					  const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeRect(*ecs, x, y, w, h, style, name) : entt::null;
}

entt::entity makeEllipse(float x, float y, float w, float h,
						 const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeEllipse(*ecs, x, y, w, h, style, name) : entt::null;
}

entt::entity makeCircle(float x, float y, float radius, const rigkit::ecs::CDrawStyle& style,
						const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeCircle(*ecs, x, y, radius, style, name) : entt::null;
}

entt::entity makeLine(float x1, float y1, float x2, float y2,
					  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeLine(*ecs, x1, y1, x2, y2, style, name) : entt::null;
}

entt::entity makeRegularPolygon(float x, float y, float radius, int sides, float rotation,
								const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeRegularPolygon(*ecs, x, y, radius, sides, rotation, style, name)
			   : entt::null;
}

entt::entity makeStar(float x, float y, float radius, float innerRadius, int points,
					  float rotation, const rigkit::ecs::CDrawStyle& style,
					  const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeStar(*ecs, x, y, radius, innerRadius, points, rotation, style, name)
			   : entt::null;
}

entt::entity makePolygon(std::vector<glm::vec2> points, bool closed,
						 const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makePolygon(*ecs, std::move(points), closed, style, name) : entt::null;
}

entt::entity makeArc(float x, float y, float radius, float startAngle, float endAngle, bool pie,
					 const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeArc(*ecs, x, y, radius, startAngle, endAngle, pie, style, name) : entt::null;
}

entt::entity makeRing(float x, float y, float outerRadius, float innerRadius,
					  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeRing(*ecs, x, y, outerRadius, innerRadius, style, name) : entt::null;
}

entt::entity makeMesh(std::vector<glm::vec3> positions, std::vector<uint32_t> indices,
					  rigkit::ecs::CMesh::Mode mode, const rigkit::ecs::CDrawStyle& style,
					  const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeMesh(*ecs, std::move(positions), std::move(indices), mode, style, name)
			   : entt::null;
}

entt::entity makeMeshTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c,
							  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeMeshTriangle(*ecs, a, b, c, style, name) : entt::null;
}

entt::entity makeMeshQuad(float x, float y, float w, float h,
						  const rigkit::ecs::CDrawStyle& style, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeMeshQuad(*ecs, x, y, w, h, style, name) : entt::null;
}

entt::entity makeMeshBox(float size, const rigkit::ecs::CDrawStyle& style,
						 const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeMeshBox(*ecs, size, style, name) : entt::null;
}

entt::entity makeSphere(float radius, int subdivisions, const rigkit::ecs::CDrawStyle& style,
						const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeSphere(*ecs, radius, subdivisions, style, name) : entt::null;
}

entt::entity makeCamera(glm::vec3 position, bool active, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeCamera(*ecs, position, active, name) : entt::null;
}

entt::entity makeLight(glm::vec3 position, rigkit::ecs::CLight::Type type,
					   const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeLight(*ecs, position, type, name) : entt::null;
}

entt::entity makePalette(rigkit::ecs::CPalette palette, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makePalette(*ecs, std::move(palette), name) : entt::null;
}

entt::entity makeIndexedAtlas(int width, int height, uint8_t fillIndex, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeIndexedAtlas(*ecs, width, height, fillIndex, name) : entt::null;
}

entt::entity makeGroup(glm::vec3 position, const std::string& name) {
	auto* ecs = boundEcs();
	return ecs ? makeGroup(*ecs, position, name) : entt::null;
}

void parentTo(entt::entity child, entt::entity parent) {
	if (auto* ecs = boundEcs()) {
		parentTo(*ecs, child, parent);
	}
}

void select(entt::entity entity, bool selected) {
	if (auto* ecs = boundEcs()) {
		select(*ecs, entity, selected);
	}
}

} // namespace rig
