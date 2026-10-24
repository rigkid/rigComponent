#pragma once

#include <cstdint>
#include <vector>

namespace rigkit {
namespace ecs {

/**
 * @brief Draw order of fill / stroke / markers. Speaks `rig.paint.order`.
 */
struct CPaintOrder {
	static constexpr bool kNoProperties = true;

	enum class Layer : uint8_t { Fill, Stroke, Markers };
	std::vector<Layer> layers;
};

} // namespace ecs
} // namespace rigkit
