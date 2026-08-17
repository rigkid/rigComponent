#include <spdlog/spdlog.h>

#include "rigComponent.h"

#include "CArc.h"
#include "CSpline.h"
#include "CSpline3d.h"
#include "CNurbsSurface.h"
#include "CCadBox.h"
#include "CCadCylinder.h"
#include "CCadSphere.h"
#include "CCadExtrude.h"
#include "CCadRevolve.h"
#include "CCadBoolean.h"
#include "CCadFillet.h"
#include "CCadChamfer.h"
#include "CAssetRef.h"
#include "CCamera.h"
#include "CCanvas.h"
#include "CCastReceiver.h"
#include "CCode.h"
#include "CScreenCast.h"
#include "CScreenCastReceive.h"
#include "CDriveHint.h"
#include "CEllipse.h"
#include "CModBinding.h"
#include "CModLfo.h"
#include "CMusicClock.h"
#include "CMusicTransport.h"
#include "COrbitDrive.h"
#include "CTween.h"
#include "CSelectable.h"
#include "CDrawStyle.h"
#include "CGroup.h"
#include "CLight.h"
#include "CFaceSelection.h"
#include "CVertexSelection.h"
#include "CEdgeSelection.h"
#include "CIndexedAtlas.h"
#include "CPalette.h"
#include "CMesh.h"
#include "CLayer.h"
#include "CLine.h"
#include "CPath.h"
#include "CCurve.h"
#include "CPolygon.h"
#include "CRectangle.h"
#include "CRegularPolygon.h"
#include "CRelationship.h"
#include "CRing.h"
#include "CSelection.h"
#include "CSphere.h"
#include "CStar.h"
#include "CText.h"
#include "CTransform.h"
#include "rig/create.h"

#include "core/RigKitEngine.h"
#include "core/pack/PackRegistry.h"
#include "ecs/MEcs.h"
#include "ecs/components/CGuide.h"

#if __has_include("rigProject.h")
#include "ComponentSerializers.h"
#include "rigProject.h"
#endif

namespace rigkit {

rigComponent::rigComponent() : IPack("rigComponent") {}

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
	ecs->registerComponent<ecs::CModLfo>("ModLfo", true);
	ecs->registerComponent<ecs::CModBinding>("ModBinding", true);
	ecs->registerComponent<ecs::CTween>("Tween", true);
	ecs->registerComponent<ecs::CMusicClock>("MusicClock", true);
	ecs->registerComponent<ecs::CMusicTransport>("MusicTransport", true);
	ecs->registerComponent<ecs::CPalette>("Palette", true);
	ecs->registerComponent<ecs::CIndexedAtlas>("IndexedAtlas", true);
	ecs->registerComponent<ecs::CRectangle>("Rectangle", true);
	ecs->registerComponent<ecs::CEllipse>("Ellipse", true);
	ecs->registerComponent<ecs::CLine>("Line", true);
	ecs->registerComponent<ecs::CPolygon>("Polygon", true);
	ecs->registerComponent<ecs::CRegularPolygon>("RegularPolygon", true);
	ecs->registerComponent<ecs::CStar>("Star", true);
	ecs->registerComponent<ecs::CArc>("Arc", true);
	ecs->registerComponent<ecs::CSpline>("Spline", true);
	ecs->registerComponent<ecs::CSpline3d>("Spline3d", true);
	ecs->registerComponent<ecs::CNurbsSurface>("NurbsSurface", true);
	ecs->registerComponent<ecs::CCadBox>("CadBox", true);
	ecs->registerComponent<ecs::CCadCylinder>("CadCylinder", true);
	ecs->registerComponent<ecs::CCadSphere>("CadSphere", true);
	ecs->registerComponent<ecs::CCadExtrude>("CadExtrude", true);
	ecs->registerComponent<ecs::CCadRevolve>("CadRevolve", true);
	ecs->registerComponent<ecs::CCadBoolean>("CadBoolean", true);
	ecs->registerComponent<ecs::CCadFillet>("CadFillet", true);
	ecs->registerComponent<ecs::CCadChamfer>("CadChamfer", true);
	ecs->registerComponent<ecs::CRing>("Ring", true);
	ecs->registerComponent<ecs::CPath>("Path", true);
	ecs->registerComponent<ecs::CCurve>("Curve", true);
	ecs->registerComponent<ecs::CMesh>("Mesh", true);
	ecs->registerComponent<ecs::CSphere>("Sphere", true);
	ecs->registerComponent<ecs::CDrawStyle>("DrawStyle", true);
	ecs->registerComponent<ecs::CSelection>("Selection", true);
	ecs->registerComponent<ecs::CSelectable>("Selectable", true);
	ecs->registerComponent<ecs::CGroup>("Group", true);
	ecs->registerComponent<ecs::CLayer>("Layer", true);
	ecs->registerComponent<ecs::CFaceSelection>("FaceSelection", true);
	ecs->registerComponent<ecs::CVertexSelection>("VertexSelection", true);
	ecs->registerComponent<ecs::CEdgeSelection>("EdgeSelection", true);
	ecs->registerComponent<ecs::CGuide>("Guide", true);
	ecs->registerComponent<ecs::CAssetRef>("AssetRef", true);
	ecs->registerComponent<ecs::CCode>("Code", true);
	ecs->registerComponent<ecs::CText>("Text", true);
	ecs->registerComponent<ecs::CScreenCast>("ScreenCast", true);
	ecs->registerComponent<ecs::CScreenCastReceive>("ScreenCastReceive", true);
	ecs->registerComponent<ecs::CCastReceiver>("CastReceiver", true);
	// Canvas POD lives here; still non-portable until a rigProject codec lands.
	ecs->registerComponent<ecs::CCanvas>("Canvas", false);

#if __has_include("rigProject.h")
	// Soft: document pack may be absent. No pack.json edge — rigProject depends
	// on us, so our setup runs first; the instance already exists for codecs.
	if (auto* doc = getPack<rigProject>("rigProject")) {
		registerComponentSerializers(*doc);
	}
#endif

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
