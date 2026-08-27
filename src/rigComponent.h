#pragma once

#include "core/pack/IPack.h"
namespace rigkit {

/// Data-only component pack. setup() only calls registerComponent — no systems.
class rigComponent : public IPack {
  public:
	rigComponent();
	bool init() override;
	void setup() override;
};

} // namespace rigkit
