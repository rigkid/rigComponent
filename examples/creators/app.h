#pragma once
#include "core/U_core.h"

class CreatorsApp : public rigkit::IApp {
  public:
	CreatorsApp() {
		window().width = 800;
		window().height = 600;
		window().title = "rigComponent: Creators";
	}
	void setup() override;
	void update(float) override {}
	void draw() override {}
};
