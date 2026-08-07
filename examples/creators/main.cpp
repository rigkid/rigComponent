#include <memory>
#include "app.h"
#include "core/RigKitEngine.h"

int main(int argc, char* argv[]) {
	auto app = std::make_unique<CreatorsApp>();
	rigkit::RigKitEngine engine(std::move(app), {}, argc, argv);
	engine.run();
	return 0;
}
