#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief An editable text / source buffer owned by the ECS.
 * @details NO CODE JUST DATA. One entity per buffer. Producers (exporters,
 * generators) write `text`; editor packs list every `CCode` entity and edit it
 * in place, so the component - not a file and not an editor buffer - is the
 * source of truth. Compose `CAssetRef` on the same entity when the buffer has
 * an on-disk origin or export target (`rig.media.asset_ref`). `name` / `order`
 * stay host chrome until `rig.meta.named`. `dirty` / `epoch` are editor cache
 * - do not serialize (see port-map).
 */
struct CCode {
	std::string name = "untitled";
	std::string text;
	std::string language; ///< Lowercase id: "gcode", "svg", "python", "" = plain.
	bool readOnly = false;
	int order = 0;

	/// Set by the editor on edit, cleared by producers on regenerate.
	bool dirty = false;
	/// Bumped whenever `text` is replaced by a producer, so editors reload.
	uint32_t epoch = 0;

	/// `text` is deliberately absent - buffers are too large for inspector rows.
	std::vector<sProp> GetProperties() {
		return {{0, "Name", EPT_STRING, &name},
				{1, "Language", EPT_STRING, &language},
				{2, "Read Only", EPT_BOOL, &readOnly},
				{3, "Order", EPT_INT, &order},
				{4, "Dirty", EPT_BOOL, &dirty}};
	}
};

} // namespace ecs
} // namespace rigkit
