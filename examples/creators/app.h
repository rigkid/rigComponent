#pragma once
#include "core/U_core.h"
class CreatorsApp : public rigkit::IApp {
  public:
	void setup() override;
	void update(float) override {}
	void draw() override {}
};
