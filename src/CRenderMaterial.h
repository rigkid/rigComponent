#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Shading parameters for a mesh. Speaks `rig.render.material`.
 * @details Compose on the same entity as `CMesh` or `CSphere`. Absent means the
 * Lambert present (face color, palette, or `CDrawStyle` fill). `albedoMap` is an
 * entity with `CAssetRef` (image). GPU programs stay in the present pack.
 * The map id is not a property-grid field; floats are.
 */
struct CRenderMaterial {
	float albedoR = 1.f;
	float albedoG = 1.f;
	float albedoB = 1.f;
	entt::entity albedoMap{entt::null};
	float metallic = 0.f;
	float roughness = 1.f;
	float emissiveR = 0.f;
	float emissiveG = 0.f;
	float emissiveB = 0.f;

	glm::vec3 albedo() const { return {albedoR, albedoG, albedoB}; }
	glm::vec3 emissive() const { return {emissiveR, emissiveG, emissiveB}; }

	std::vector<sProp> GetProperties() {
		return {{0, "Albedo R", EPT_FLOAT, &albedoR},
				{1, "Albedo G", EPT_FLOAT, &albedoG},
				{2, "Albedo B", EPT_FLOAT, &albedoB},
				{3, "Metallic", EPT_FLOAT, &metallic},
				{4, "Roughness", EPT_FLOAT, &roughness},
				{5, "Emissive R", EPT_FLOAT, &emissiveR},
				{6, "Emissive G", EPT_FLOAT, &emissiveG},
				{7, "Emissive B", EPT_FLOAT, &emissiveB}};
	}
};

} // namespace ecs
} // namespace rigkit
