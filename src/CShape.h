#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/** @brief Portable geometric shape POD (bbox). Host union — Rig split into geometry primitives. */
struct CShape {
	enum class Type { Rectangle, Ellipse, Line, Polygon, Star };

	Type type = Type::Rectangle;
	float x1 = 0.0f, y1 = 0.0f, x2 = 100.0f, y2 = 100.0f;
	int sides = 5;
	float innerRadius = 0.5f;

	float getWidth() const { return x2 - x1; }
	float getHeight() const { return y2 - y1; }
	void setPosition(float x, float y) {
		x1 = x;
		y1 = y;
	}
	void setSize(float width, float height) {
		x2 = x1 + width;
		y2 = y1 + height;
	}

	std::vector<sProp> GetProperties() {
		return {{0, "Start X", EPT_FLOAT, &x1}, {1, "Start Y", EPT_FLOAT, &y1},
				{2, "End X", EPT_FLOAT, &x2},	{3, "End Y", EPT_FLOAT, &y2},
				{4, "Sides", EPT_INT, &sides},	{5, "Inner Radius", EPT_FLOAT, &innerRadius}};
	}
};

} // namespace ecs
} // namespace rigkit
