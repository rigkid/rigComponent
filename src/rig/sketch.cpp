#include "rig/sketch.h"

#include "CTransform.h"
#include "rig/create.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace rig {
namespace {

struct Runtime {
	std::vector<entt::entity> entities;
	/// ECS the tracked entities were created in. entt handles are only
	/// meaningful against their own registry (the same index/version can be
	/// `valid()` in another one), so tracking drops on `setEcs` rebind rather
	/// than ever destroying across registries.
	rigkit::MEcs* trackEcs = nullptr;
	bool hasFill = true;
	bool hasStroke = false;
	float fillR = 1.f, fillG = 1.f, fillB = 1.f, fillA = 1.f;
	float strokeR = 0.f, strokeG = 0.f, strokeB = 0.f, strokeA = 1.f;
	float strokeWidth = 1.f;
	float fillOpacity = 1.f;
	glm::mat4 model{1.f};
	std::vector<glm::mat4> matrixStack;
	std::vector<rigkit::ecs::CPath::Command> path;
	SketchEntityHook hook = nullptr;
};

Runtime g_rt;

void resetStyle() {
	g_rt.hasFill = true;
	g_rt.hasStroke = false;
	g_rt.fillR = g_rt.fillG = g_rt.fillB = g_rt.fillA = 1.f;
	g_rt.strokeR = g_rt.strokeG = g_rt.strokeB = 0.f;
	g_rt.strokeA = 1.f;
	g_rt.strokeWidth = 1.f;
	g_rt.fillOpacity = 1.f;
	g_rt.model = glm::mat4(1.f);
	g_rt.matrixStack.clear();
	g_rt.path.clear();
}

/**
 * Put the sketch matrix (composed with a local offset) on the entity's
 * transform. Shapes stay parametric in local coordinates; the 2D affine
 * decomposes exactly as T * R * Sh * S (the `CTransform::localMatrix` order),
 * so rotation, anisotropic scale and shear all survive - nothing is baked
 * into geometry and nothing is approximated.
 */
void applyModelTo(entt::entity e, float localX, float localY) {
	auto* ecs = currentEcs();
	if (!ecs || e == entt::null || !ecs->hasComponent<rigkit::ecs::CTransform>(e)) {
		return;
	}
	const glm::mat4 m =
		g_rt.model * glm::translate(glm::mat4(1.f), glm::vec3(localX, localY, 0.f));
	// QR-style split of the linear part: a = image of x-axis, b = image of y-axis.
	const glm::vec2 a{m[0][0], m[0][1]};
	const glm::vec2 b{m[1][0], m[1][1]};
	float sx = glm::length(a);
	float theta = 0.f;
	glm::vec2 dir{1.f, 0.f};
	if (sx > 1e-8f) {
		dir = a / sx;
		theta = std::atan2(dir.y, dir.x);
	} else {
		sx = 1e-8f; // Degenerate x-axis - keep the transform invertible.
	}
	const glm::vec2 perp{-dir.y, dir.x};
	const float u01 = glm::dot(b, dir);  // = shear * sy
	float sy = glm::dot(b, perp);        // negative = reflection, that's fine
	if (std::fabs(sy) < 1e-8f) {
		sy = (sy < 0.f) ? -1e-8f : 1e-8f;
	}
	auto& t = ecs->getComponent<rigkit::ecs::CTransform>(e);
	t.position = {m[3][0], m[3][1], 0.f};
	t.setEulerRadians({0.f, 0.f, theta});
	t.scale = {sx, sy, 1.f};
	t.shear = {u01 / sy, 0.f, 0.f};
	t.world = t.localMatrix(); // Same refresh the creators do at spawn.
}

rigkit::ecs::CDrawStyle currentStyle() {
	rigkit::ecs::CDrawStyle s;
	s.hasFill = g_rt.hasFill;
	s.hasStroke = g_rt.hasStroke;
	s.setFillColor(g_rt.fillR, g_rt.fillG, g_rt.fillB, g_rt.fillA * g_rt.fillOpacity);
	s.setStrokeColor(g_rt.strokeR, g_rt.strokeG, g_rt.strokeB, g_rt.strokeA);
	s.strokeWidth = g_rt.strokeWidth;
	return s;
}

bool hasPaint() {
	return g_rt.hasFill || g_rt.hasStroke;
}

rigkit::ecs::CDrawStyle lineStyle() {
	if (g_rt.hasStroke) {
		return stroke(g_rt.strokeR, g_rt.strokeG, g_rt.strokeB, g_rt.strokeA, g_rt.strokeWidth);
	}
	return stroke(g_rt.fillR, g_rt.fillG, g_rt.fillB, g_rt.fillA * g_rt.fillOpacity,
				  g_rt.strokeWidth);
}

entt::entity track(entt::entity e) {
	if (e == entt::null) {
		return e;
	}
	auto* ecs = currentEcs();
	if (ecs != g_rt.trackEcs) {
		// Rebound since the last shape: earlier entities belong to another
		// document. Stop tracking them (they live on there).
		g_rt.entities.clear();
		g_rt.trackEcs = ecs;
	}
	g_rt.entities.push_back(e);
	if (g_rt.hook) {
		g_rt.hook(e);
	}
	return e;
}

} // namespace

void setSketchEntityHook(SketchEntityHook hook) {
	g_rt.hook = hook;
}

void resetSketch() {
	resetStyle();
}

void clearSketch() {
	auto* ecs = currentEcs();
	if (ecs && ecs == g_rt.trackEcs) {
		for (auto e : g_rt.entities) {
			if (ecs->registry().valid(e)) {
				ecs->destroyEntity(e);
			}
		}
	}
	g_rt.entities.clear();
	g_rt.trackEcs = nullptr;
	resetStyle();
}

int sketchShapeCount() {
	if (currentEcs() != g_rt.trackEcs) {
		return 0; // Tracked shapes belong to a previously bound ECS.
	}
	return static_cast<int>(g_rt.entities.size());
}

void setFill(float r, float g, float b, float a) {
	g_rt.hasFill = true;
	g_rt.fillR = r;
	g_rt.fillG = g;
	g_rt.fillB = b;
	g_rt.fillA = a;
}

void setStroke(float r, float g, float b, float a) {
	g_rt.hasStroke = true;
	g_rt.strokeR = r;
	g_rt.strokeG = g;
	g_rt.strokeB = b;
	g_rt.strokeA = a;
}

void setStrokeWidth(float width) {
	g_rt.strokeWidth = width;
}

void setFillOpacity(float opacity) {
	g_rt.fillOpacity = opacity;
}

void noFill() {
	g_rt.hasFill = false;
}

void noStroke() {
	g_rt.hasStroke = false;
}

void pushMatrix() {
	g_rt.matrixStack.push_back(g_rt.model);
}

void popMatrix() {
	if (!g_rt.matrixStack.empty()) {
		g_rt.model = g_rt.matrixStack.back();
		g_rt.matrixStack.pop_back();
	}
}

void resetMatrix() {
	g_rt.model = glm::mat4(1.f);
	g_rt.matrixStack.clear();
}

void translate(float x, float y) {
	g_rt.model = glm::translate(g_rt.model, glm::vec3(x, y, 0.f));
}

void rotate(float radians) {
	g_rt.model = glm::rotate(g_rt.model, radians, glm::vec3(0.f, 0.f, 1.f));
}

void scale(float s) {
	scale(s, s);
}

void scale(float sx, float sy) {
	g_rt.model = glm::scale(g_rt.model, glm::vec3(sx, sy, 1.f));
}

entt::entity circle(float x, float y, float radius) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeCircle(0.f, 0.f, radius, currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

entt::entity rect(float x, float y, float w, float h) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeRect(0.f, 0.f, w, h, currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

entt::entity ellipse(float x, float y, float w, float h) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeEllipse(0.f, 0.f, w, h, currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

entt::entity oval(float x, float y, float w, float h) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeEllipse(0.f, 0.f, w, h, currentStyle());
	applyModelTo(e, x + w * 0.5f, y + h * 0.5f);
	return track(e);
}

entt::entity line(float x1, float y1, float x2, float y2) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeLine(x1, y1, x2, y2, lineStyle());
	applyModelTo(e, 0.f, 0.f);
	return track(e);
}

entt::entity triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makePolygon({{x1, y1}, {x2, y2}, {x3, y3}}, true, currentStyle());
	applyModelTo(e, 0.f, 0.f);
	return track(e);
}

entt::entity polygon(const std::vector<glm::vec2>& points) {
	if (!currentEcs() || !hasPaint() || points.size() < 3) {
		return entt::null;
	}
	auto e = makePolygon(points, true, currentStyle());
	applyModelTo(e, 0.f, 0.f);
	return track(e);
}

entt::entity regularPolygon(float x, float y, float radius, int sides, float rotationRadians) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	// Creators speak degrees; the sketch layer is radians throughout.
	auto e = makeRegularPolygon(0.f, 0.f, radius, sides, glm::degrees(rotationRadians),
								currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

entt::entity star(float x, float y, float radius, float innerRadius, int points,
				  float rotationRadians) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeStar(0.f, 0.f, radius, innerRadius, points, glm::degrees(rotationRadians),
					  currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

entt::entity arc(float x, float y, float radius, float startRadians, float endRadians, bool pie) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeArc(0.f, 0.f, radius, glm::degrees(startRadians), glm::degrees(endRadians), pie,
					 currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

entt::entity ring(float x, float y, float outerRadius, float innerRadius) {
	if (!currentEcs() || !hasPaint()) {
		return entt::null;
	}
	auto e = makeRing(0.f, 0.f, outerRadius, innerRadius, currentStyle());
	applyModelTo(e, x, y);
	return track(e);
}

void beginPath() {
	g_rt.path.clear();
}

void moveTo(float x, float y) {
	rigkit::ecs::CPath::Command cmd;
	cmd.type = rigkit::ecs::CPath::Cmd::MoveTo;
	cmd.p = {x, y};
	g_rt.path.push_back(cmd);
}

void lineTo(float x, float y) {
	if (g_rt.path.empty()) {
		moveTo(x, y);
		return;
	}
	rigkit::ecs::CPath::Command cmd;
	cmd.type = rigkit::ecs::CPath::Cmd::LineTo;
	cmd.p = {x, y};
	g_rt.path.push_back(cmd);
}

void curveTo(float c1x, float c1y, float c2x, float c2y, float x, float y) {
	if (g_rt.path.empty()) {
		moveTo(x, y);
		return;
	}
	// A true cubic - control points ride the command; renderers flatten for
	// display only, the document keeps the curve.
	rigkit::ecs::CPath::Command cmd;
	cmd.type = rigkit::ecs::CPath::Cmd::CubicTo;
	cmd.c1 = {c1x, c1y};
	cmd.c2 = {c2x, c2y};
	cmd.p = {x, y};
	g_rt.path.push_back(cmd);
}

void closePath() {
	if (g_rt.path.empty()) {
		return;
	}
	rigkit::ecs::CPath::Command cmd;
	cmd.type = rigkit::ecs::CPath::Cmd::Close;
	g_rt.path.push_back(cmd);
}

entt::entity drawPath() {
	if (!currentEcs() || g_rt.path.size() < 2 || !hasPaint()) {
		return entt::null;
	}
	rigkit::ecs::CPath path;
	path.commands = g_rt.path;
	auto e = makePath(std::move(path),
					  g_rt.hasFill ? currentStyle() : lineStyle());
	applyModelTo(e, 0.f, 0.f);
	return track(e);
}

} // namespace rig
