#pragma once

#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Generic layer tag for hierarchy / Layers panel (visibility + order).
 * @details NO CODE JUST DATA. Parenting stays on `CRelationship`.
 * Speaks `rig.spatial.layer` (tint may be four floats).
 */
struct CLayer {
	bool visible = true;
	bool locked = false;
	int order = 0;
	float colorR = 0.55f;
	float colorG = 0.55f;
	float colorB = 0.55f;
	float colorA = 1.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Visible", EPT_BOOL, &visible},
				{1, "Locked", EPT_BOOL, &locked},
				{2, "Order", EPT_INT, &order},
				{3, "Color R", EPT_FLOAT, &colorR},
				{4, "Color G", EPT_FLOAT, &colorG},
				{5, "Color B", EPT_FLOAT, &colorB},
				{6, "Color A", EPT_FLOAT, &colorA}};
	}
};

} // namespace ecs
} // namespace rigkit
