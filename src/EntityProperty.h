#pragma once

/**
 * @file
 * @brief Reads and writes reflected entity properties by name.
 * @details Numeric scalars only, resolved through `GetProperties`. Dotted contract keys such as
 * `position.y` map onto the display names the components register.
 */

#include "ecs/MEcs.h"

#include <glm/glm.hpp>
#include <optional>
#include <string_view>

namespace rigkit {
namespace ecs {

/**
 * @brief Write a reflected float/int/bool property on an entity by name.
 * @details Resolves `propertyKey` against `GetProperties` / registered props.
 * Accepts dotted contract keys (`position.y` → `Position Y`) and exact prop names.
 * @return True when a matching writable property was found.
 */
bool writeEntityProperty(MEcs& ecs, entt::entity entity, std::string_view propertyKey, float value);

/** @brief Resolve entity by name, then writeEntityProperty. */
bool writeEntityProperty(MEcs& ecs, std::string_view entityName, std::string_view propertyKey,
						 float value);

/** @brief Read a reflected float (int/bool coerced) if present. */
std::optional<float> readEntityProperty(MEcs& ecs, entt::entity entity,
										std::string_view propertyKey);

std::optional<float> readEntityProperty(MEcs& ecs, std::string_view entityName,
										std::string_view propertyKey);

/** @brief Write two floats into a vec2 property, or two named float props. */
bool writeEntityVec2Property(MEcs& ecs, entt::entity entity, std::string_view propertyKey,
							 glm::vec2 value);

} // namespace ecs
} // namespace rigkit
