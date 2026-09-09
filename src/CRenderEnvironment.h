#pragma once

#include <entt/entt.hpp>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Scene image-based light. Speaks `rig.render.environment`.
 * @details One entity is enough. `hdri` is a `CAssetRef` (lat-long Radiance
 * image). `intensity` scales the result; absent on the wire is 1. Decode and
 * the spherical-harmonic projection stay in the present pack. Coefficients
 * are a host cache. The asset id is not a property-grid field.
 */
struct CRenderEnvironment {
	entt::entity hdri{entt::null};
	float intensity = 1.f;

	std::vector<sProp> GetProperties() {
		return {{0, "Intensity", EPT_FLOAT, &intensity}};
	}
};

} // namespace ecs
} // namespace rigkit
