#pragma once

#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief File identity — data only. Speaks `rig.media.asset_ref`.
 * @details One schema for paths. Other components hold an entity id that
 * points here (`CText::font`, …) or compose this type on the same entity
 * (`CCode` + disk origin). Decode / GPU / players stay in fulfillment packs.
 * `loop` is meaningful for playback kinds; ignore it for stills and fonts.
 */
struct CAssetRef {
	enum class Kind : int {
		Image = 0,
		Audio = 1,
		Video = 2,
		Model = 3,
		Font = 4,
		Document = 5,
		Other = 6
	};

	Kind kind = Kind::Other;
	std::string path;
	bool loop = false;

	std::vector<sProp> GetProperties() {
		static const char* const kKindNames[] = {"image", "audio", "video", "model",
												 "font", "document", "other"};
		return {{0, "Kind", EPT_ENUM, reinterpret_cast<int*>(&kind), kKindNames, 7},
				{1, "Path", EPT_STRING, &path},
				{2, "Loop", EPT_BOOL, &loop}};
	}
};

} // namespace ecs
} // namespace rigkit
