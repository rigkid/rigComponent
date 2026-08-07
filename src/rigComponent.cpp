#include <spdlog/spdlog.h>

#include "rigComponent.h"

#include "CAssetRef.h"
#include "CCamera.h"
#include "CCanvas.h"
#include "CCode.h"
#include "CDriveHint.h"
#include "COrbitDrive.h"
#include "CSelectable.h"
#include "CDrawStyle.h"
#include "CGroup.h"
#include "CLight.h"
#include "CFaceSelection.h"
#include "CIndexedAtlas.h"
#include "CPalette.h"
#include "CMesh.h"
#include "CLayer.h"
#include "CPath.h"
#include "CRelationship.h"
#include "CSelection.h"
#include "CShape.h"
#include "CText.h"
#include "CTransform.h"
#include "rig/create.h"

#include "core/RigKitEngine.h"
#include "core/pack/PackRegistry.h"
#include "ecs/MEcs.h"
#include "ecs/components/CGuide.h"

namespace rigkit {

rigComponent::rigComponent() : IPack("rigComponent") {
	setDescription("Data-only ECS components (NO CODE JUST DATA)");
}

bool rigComponent::init() {
	spdlog::info("[rigComponent] init (data-only pack)");
	return true;
}

void rigComponent::setup() {
	auto* engine = getEngine();
	if (!engine) {
		return;
	}
	auto* ecs = engine->getECSManager();
	if (!ecs) {
		return;
	}

	// Bare rig::make* creators write here, so sketches never plumb an MEcs.
	rig::setEcs(ecs);

	ecs->registerComponent<ecs::CTransform>("Transform", true);
	ecs->registerComponent<ecs::CRelationship>("Relationship", true);
	ecs->registerComponent<ecs::CCamera>("Camera", true);
	ecs->registerComponent<ecs::CLight>("Light", true);
	ecs->registerComponent<ecs::CDriveHint>("DriveHint", true);
	ecs->registerComponent<ecs::COrbitDrive>("OrbitDrive", true);
	ecs->registerComponent<ecs::CPalette>("Palette", true);
	ecs->registerComponent<ecs::CIndexedAtlas>("IndexedAtlas", true);
	ecs->registerComponent<ecs::CShape>("Shape", true);
	ecs->registerComponent<ecs::CPath>("Path", true);
	ecs->registerComponent<ecs::CMesh>("Mesh", true);
	ecs->registerComponent<ecs::CDrawStyle>("DrawStyle", true);
	ecs->registerComponent<ecs::CSelection>("Selection", true);
	ecs->registerComponent<ecs::CSelectable>("Selectable", true);
	ecs->registerComponent<ecs::CGroup>("Group", true);
	ecs->registerComponent<ecs::CLayer>("Layer", true);
	ecs->registerComponent<ecs::CFaceSelection>("FaceSelection", true);
	ecs->registerComponent<ecs::CGuide>("Guide", true);
	ecs->registerComponent<ecs::CAssetRef>("AssetRef", true);
	ecs->registerComponent<ecs::CCode>("Code", true);
	ecs->registerComponent<ecs::CText>("Text", true);
	// Canvas POD lives here; still non-portable until a rigProject codec lands.
	ecs->registerComponent<ecs::CCanvas>("Canvas", false);

	spdlog::info("[rigComponent] registered {} component types", ecs->componentTypes().size());
}

} // namespace rigkit

namespace {
struct rigComponentRegistrar {
	rigComponentRegistrar() {
		rigkit::PackRegistry::instance().addFactory("rigComponent", []() {
			return std::shared_ptr<rigkit::IPack>(std::make_shared<rigkit::rigComponent>());
		});
	}
};
static rigComponentRegistrar rigComponent_auto_reg;
} // namespace
