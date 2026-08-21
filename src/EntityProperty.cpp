#include "EntityProperty.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <unordered_map>

namespace rigkit {
namespace ecs {
namespace {

std::string normalizeKey(std::string_view key) {
	std::string out;
	out.reserve(key.size());
	for (char c : key) {
		if (c == '.' || c == ' ' || c == '_' || c == '-') {
			continue;
		}
		out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
	}
	return out;
}

const std::unordered_map<std::string, std::string>& aliasMap() {
	static const std::unordered_map<std::string, std::string> k = {
		{"positionx", "Position X"},
		{"positiony", "Position Y"},
		{"positionz", "Position Z"},
		{"scalex", "Scale X"},
		{"scaley", "Scale Y"},
		{"scalez", "Scale Z"},
		{"rotationx", "Rotation X (rad)"},
		{"rotationy", "Rotation Y (rad)"},
		{"rotationz", "Rotation Z (rad)"},
		{"colorr", "Color R"},
		{"colorg", "Color G"},
		{"colorb", "Color B"},
		{"colora", "Color A"},
	};
	return k;
}

std::string resolvePropName(std::string_view propertyKey) {
	const std::string norm = normalizeKey(propertyKey);
	const auto& aliases = aliasMap();
	if (auto it = aliases.find(norm); it != aliases.end()) {
		return it->second;
	}
	return std::string(propertyKey);
}

bool namesMatch(std::string_view propName, std::string_view requested) {
	if (propName == requested) {
		return true;
	}
	return normalizeKey(propName) == normalizeKey(requested);
}

} // namespace

bool writeEntityProperty(MEcs& ecs, entt::entity entity, std::string_view propertyKey, float value) {
	if (entity == entt::null || propertyKey.empty()) {
		return false;
	}
	const std::string resolved = resolvePropName(propertyKey);
	for (const auto& typeInfo : ecs.componentTypes()) {
		if (!ecs.hasRegisteredComponent(typeInfo, entity)) {
			continue;
		}
		auto props = ecs.registeredProperties(typeInfo, entity);
		for (auto& p : props) {
			if (!namesMatch(p.name, resolved) && !namesMatch(p.name, propertyKey)) {
				continue;
			}
			if (p.type == EPT_FLOAT || p.type == EPT_DOUBLE) {
				*static_cast<float*>(p.data) = value;
				return true;
			}
			if (p.type == EPT_INT || p.type == EPT_UINT) {
				*static_cast<int*>(p.data) = static_cast<int>(std::lround(value));
				return true;
			}
			if (p.type == EPT_BOOL) {
				*static_cast<bool*>(p.data) = value >= 0.5f;
				return true;
			}
		}
	}
	return false;
}

bool writeEntityProperty(MEcs& ecs, std::string_view entityName, std::string_view propertyKey,
						 float value) {
	if (entityName.empty()) {
		return false;
	}
	const entt::entity e = ecs.findEntity(std::string(entityName));
	return writeEntityProperty(ecs, e, propertyKey, value);
}

std::optional<float> readEntityProperty(MEcs& ecs, entt::entity entity,
										std::string_view propertyKey) {
	if (entity == entt::null || propertyKey.empty()) {
		return std::nullopt;
	}
	const std::string resolved = resolvePropName(propertyKey);
	for (const auto& typeInfo : ecs.componentTypes()) {
		if (!ecs.hasRegisteredComponent(typeInfo, entity)) {
			continue;
		}
		auto props = ecs.registeredProperties(typeInfo, entity);
		for (auto& p : props) {
			if (!namesMatch(p.name, resolved) && !namesMatch(p.name, propertyKey)) {
				continue;
			}
			if (p.type == EPT_FLOAT || p.type == EPT_DOUBLE) {
				return *static_cast<float*>(p.data);
			}
			if (p.type == EPT_INT || p.type == EPT_UINT) {
				return static_cast<float>(*static_cast<int*>(p.data));
			}
			if (p.type == EPT_BOOL) {
				return *static_cast<bool*>(p.data) ? 1.f : 0.f;
			}
		}
	}
	return std::nullopt;
}

std::optional<float> readEntityProperty(MEcs& ecs, std::string_view entityName,
										std::string_view propertyKey) {
	if (entityName.empty()) {
		return std::nullopt;
	}
	const entt::entity e = ecs.findEntity(std::string(entityName));
	return readEntityProperty(ecs, e, propertyKey);
}

bool writeEntityVec2Property(MEcs& ecs, entt::entity entity, std::string_view propertyKey,
							 glm::vec2 value) {
	if (entity == entt::null || propertyKey.empty()) {
		return false;
	}
	const std::string resolved = resolvePropName(propertyKey);
	for (const auto& typeInfo : ecs.componentTypes()) {
		if (!ecs.hasRegisteredComponent(typeInfo, entity)) {
			continue;
		}
		auto props = ecs.registeredProperties(typeInfo, entity);
		for (auto& p : props) {
			if (!namesMatch(p.name, resolved) && !namesMatch(p.name, propertyKey)) {
				continue;
			}
			if (p.type == EPT_VEC2) {
				auto* v = static_cast<float*>(p.data);
				v[0] = value.x;
				v[1] = value.y;
				return true;
			}
		}
	}
	return false;
}

} // namespace ecs
} // namespace rigkit
