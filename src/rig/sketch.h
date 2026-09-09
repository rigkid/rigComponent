#pragma once

/**
 * @file
 * @brief Retained-style sketch helpers - fill once, then spawn ECS shapes.
 * @details Artist sugar over `makeRect` / `makeCircle` and friends: one fill
 * and a matrix stack, then `circle` / `rect` write inspectable entities.
 * `circle(x, y, r)` takes radius. This layer does not know pages; rigProject
 * parents later sketch entities to `currentPage()`.
 * @ref rig::setSketchEntityHook replaces that.
 *
 * Style setters are `setFill` / `setStroke` so they stay distinct from the
 * `CDrawStyle` factories `fill` / `stroke` in `create.h`.
 *
 * **Angles are radians** throughout (Processing-style); the bare creators in
 * `create.h` keep their degree parameters.
 *
 * **Transforms live on the entity, exactly.** Shapes are created in local
 * coordinates and the sketch matrix at creation time is decomposed onto the
 * entity's `CTransform` as translate * rotate * shear * scale - any 2D affine
 * fits, so rotation, uneven scale and shear all survive with the shape still
 * parametric (a sheared rect is a `CRect` with `shear` on its transform).
 * Later matrix edits never move existing shapes.
 *
 * **State is process-global and single-threaded**, and entity tracking is per
 * bound ECS: rebinding via `rig::setEcs` drops tracking of earlier shapes
 * (they live on in their own document; `clearSketch` never reaches across
 * registries).
 *
 * @see create.h
 */

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <vector>

namespace rig {

/** @brief Called after each sketch primitive creates an entity (may be empty). */
using SketchEntityHook = std::function<void(entt::entity)>;

/**
 * @brief Install an on-created hook (page placement, logging, ...).
 * @details Pass `{}` / `nullptr` to clear. rigProject's first page call
 * installs `placeOnPage`; a later install replaces that.
 */
void setSketchEntityHook(SketchEntityHook hook);

/** @brief Reset fill/stroke, matrix stack, and path. Does not destroy entities. */
void resetSketch();

/**
 * @brief Destroy entities spawned by sketch primitives and reset style.
 * @details Does not touch pages or other document state. Only destroys
 * shapes made in the currently bound ECS - shapes tracked before a
 * `setEcs` rebind are left alone (handles must never cross registries).
 */
void clearSketch();

/** @brief How many sketch-spawned entities the currently bound ECS tracks. */
int sketchShapeCount();

void setFill(float r, float g, float b, float a = 1.0f);
void setStroke(float r, float g, float b, float a = 1.0f);
void setStrokeWidth(float width);
void setFillOpacity(float opacity);
void noFill();
void noStroke();

void pushMatrix();
void popMatrix();
void resetMatrix();
void translate(float x, float y);
/** @brief Rotate the sketch matrix. @param radians Counter-clockwise about Z. */
void rotate(float radians);
void scale(float s);
void scale(float sx, float sy);

/**
 * @brief Circle about `(x, y)`. @param radius Centre to edge, not diameter.
 * @return New entity, or `entt::null` when there is no paint or no ECS.
 */
entt::entity circle(float x, float y, float radius);
/** @brief Rectangle from top-left `(x, y)`. */
entt::entity rect(float x, float y, float w, float h);
/** @brief Ellipse about centre `(x, y)` with size `w` x `h`. */
entt::entity ellipse(float x, float y, float w, float h);
/** @brief Ellipse from top-left box `(x, y, w, h)`. */
entt::entity oval(float x, float y, float w, float h);
/** @brief Line segment. Uses the fill colour when no stroke is set. */
entt::entity line(float x1, float y1, float x2, float y2);
entt::entity triangle(float x1, float y1, float x2, float y2, float x3, float y3);
entt::entity polygon(const std::vector<glm::vec2>& points);
entt::entity regularPolygon(float x, float y, float radius, int sides,
							float rotationRadians = 0.0f);
entt::entity star(float x, float y, float radius, float innerRadius, int points = 5,
				  float rotationRadians = 0.0f);
entt::entity arc(float x, float y, float radius, float startRadians, float endRadians,
				 bool pie = false);
entt::entity ring(float x, float y, float outerRadius, float innerRadius);

void beginPath();
void moveTo(float x, float y);
void lineTo(float x, float y);
/**
 * @brief Cubic Bezier from the current point to `(x, y)`.
 * @details Stored as a true `CPath` cubic - control points survive into the
 * document; renderers flatten for display only. With an empty path it just
 * starts the path at `(x, y)`.
 */
void curveTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
void closePath();
/** @brief Emit the recorded commands as a `rig.geometry.path` entity. */
entt::entity drawPath();

} // namespace rig
