#include "rigComponent.h"

#include "core/RigKitEngine.h"
#include "core/pack/PackRegistry.h"
#include "ecs/MEcs.h"
#include "ecs/components/CGuide.h"
#include "CAnchor.h"
#include "CArc.h"
#include "CAssetRef.h"
#include "CCamera.h"
#include "CCanvas.h"
#include "CCastReceiver.h"
#include "CCode.h"
#include "CCurve.h"
#include "CDrawStyle.h"
#include "CDriveHint.h"
#include "CEdgeSelection.h"
#include "CEllipse.h"
#include "CFaceSelection.h"
#include "CGroup.h"
#include "CIndexedAtlas.h"
#include "CLayer.h"
#include "CLight.h"
#include "CLine.h"
#include "CMesh.h"
#include "CName.h"
#include "CNurbsSurface.h"
#include "COrbitDrive.h"
#include "COrthoView.h"
#include "CPalette.h"
#include "CPath.h"
#include "CPath3d.h"
#include "CPolygon.h"
#include "CRectangle.h"
#include "CRegularPolygon.h"
#include "CRelationship.h"
#include "CRenderBlend.h"
#include "CRenderEnvironment.h"
#include "CRenderMaterial.h"
#include "CRenderVisibility.h"
#include "CRing.h"
#include "CScreenCast.h"
#include "CScreenCastReceive.h"
#include "CSelectable.h"
#include "CSelection.h"
#include "CSphere.h"
#include "CSpline.h"
#include "CSpline3d.h"
#include "CStar.h"
#include "CText.h"
#include "CTransform.h"
#include "CUiPanel.h"
#include "CVertexSelection.h"
#include "rig/create.h"

#include <spdlog/spdlog.h>

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
	ecs->registerComponent<ecs::CAnchor>("Anchor", true);
	ecs->registerComponent<ecs::CRelationship>("Relationship", true);
	ecs->registerComponent<ecs::CCamera>("Camera", true);
	ecs->registerComponent<ecs::CLight>("Light", true);
	ecs->registerComponent<ecs::CDriveHint>("DriveHint", true);
	ecs->registerComponent<ecs::COrbitDrive>("OrbitDrive", true);
	ecs->registerComponent<ecs::COrthoView>("OrthoView", true);
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
	ecs->registerComponent<ecs::CRing>("Ring", true);
	ecs->registerComponent<ecs::CPath>("Path", true);
	ecs->registerComponent<ecs::CPath3d>("Path3d", true);
	ecs->registerComponent<ecs::CCurve>("Curve", true);
	ecs->registerComponent<ecs::CMesh>("Mesh", true);
	ecs->registerComponent<ecs::CSphere>("Sphere", true);
	ecs->registerComponent<ecs::CDrawStyle>("DrawStyle", true);
	ecs->registerComponent<ecs::CSelection>("Selection", true);
	ecs->registerComponent<ecs::CSelectable>("Selectable", true);
	ecs->registerComponent<ecs::CGroup>("Group", true);
	ecs->registerComponent<ecs::CLayer>("Layer", true);
	ecs->registerComponent<ecs::CName>("Name", true);
	ecs->registerComponent<ecs::CRenderVisibility>("RenderVisibility", true);
	ecs->registerComponent<ecs::CRenderBlend>("RenderBlend", true);
	ecs->registerComponent<ecs::CRenderMaterial>("RenderMaterial", true);
	ecs->registerComponent<ecs::CRenderEnvironment>("RenderEnvironment", false);
	ecs->registerComponent<ecs::CUiPanel>("UiPanel", true);
	ecs->registerComponent<ecs::CUiGroup>("UiGroup", true);
	ecs->registerComponent<ecs::CUiControl>("UiControl", true);
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
