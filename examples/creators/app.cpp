#include "app.h"

#include "core/pack/MPack.h"
#include "core/RigKitEngine.h"
#include "packs/rigComponent/src/rig.h"
#include "packs/rigComponent/src/rigComponent.h"
#include "packs/rigSystems/src/rigSystems.h"

#include <spdlog/spdlog.h>

namespace {

// Poster ink lives in CPalette, so the colours are inspectable data like the shapes.
// These are the same slot indices CMesh::facePalette would reference.
constexpr int kBlue   = 0;
constexpr int kOrange = 1;
constexpr int kAmber  = 2;
constexpr int kRed    = 3;
constexpr int kViolet = 4;
constexpr int kGreen  = 5;
constexpr int kYellow = 6;
constexpr int kPaper  = 7;
constexpr int kChalk  = 8;
constexpr int kWhite  = 9;

void setSlot(rigkit::ecs::CPalette& palette, int slot, glm::vec4 rgba) {
	palette.colors[static_cast<size_t>(slot)] = rgba;
}

/// Flat poster palette - no shade ramps,
/// so every slot keeps the CPalette default stop.
rigkit::ecs::CPalette posterPalette() {
	rigkit::ecs::CPalette p;
	setSlot(p, kBlue,   {0.20f, 0.56f, 0.96f, 1.f});
	setSlot(p, kOrange, {0.96f, 0.58f, 0.22f, 1.f});
	setSlot(p, kAmber,  {0.95f, 0.55f, 0.22f, 1.f});
	setSlot(p, kRed,    {0.90f, 0.36f, 0.28f, 1.f});
	setSlot(p, kViolet, {0.48f, 0.34f, 0.86f, 1.f});
	setSlot(p, kGreen,  {0.28f, 0.78f, 0.52f, 1.f});
	setSlot(p, kYellow, {0.96f, 0.80f, 0.20f, 1.f});
	setSlot(p, kPaper,  {0.96f, 0.96f, 0.96f, 1.f});
	setSlot(p, kChalk,  {0.94f, 0.95f, 0.97f, 1.f});
	setSlot(p, kWhite,  {1.00f, 1.00f, 1.00f, 1.f});
	return p;
}

rigkit::ecs::CDrawStyle solid(const glm::vec4& ink) {
	return rig::fill(ink.r, ink.g, ink.b, ink.a);
}

rigkit::ecs::CDrawStyle outlined(const glm::vec4& ink, const glm::vec4& edge, float edgeWidth) {
	return rig::fillAndStroke(ink.r, ink.g, ink.b, edge.r, edge.g, edge.b, edgeWidth);
}

rigkit::ecs::CDrawStyle pen(const glm::vec4& ink, float alpha, float width) {
	return rig::stroke(ink.r, ink.g, ink.b, alpha, width);
}

} // namespace

void CreatorsApp::setup() {
	spdlog::info("creators: rigComponent");
	m_engine->setClearColor(0.09f, 0.10f, 0.14f, 1.0f);

	auto* packs = m_engine->getPackManager();
	if (!packs) {
		return;
	}
	// rigComponent owns the data types and points the rig:: creators at the host ECS.
	// rigSystems presents them. Data pack first.
	packs->registerPack<rigkit::rigComponent>();
	packs->registerPack<rigkit::rigSystems>();
	packs->initAll();
	packs->setupAll();

	// The palette is an entity too, so the inspector can show and edit the poster ink.
	const auto poster = posterPalette();
	rig::makePalette(poster, "poster-palette");
	const auto ink = [&poster](int slot) { return poster.color(slot); };

	// From here it reads like a sketch. Every creator writes POD components:
	// CTransform + CShape (or CMesh) + CDrawStyle, nothing else.
	// Rectangles take a top-left corner, circles and ellipses take a centre,
	// and creation order is paint order.

	// Selection is data too - the present path strokes a bounds overlay.
	auto rect = rig::makeRect(40.f, 40.f, 220.f, 170.f, solid(ink(kBlue)), "demo-rect");
	rig::select(rect);

	rig::makeRect(280.f, 40.f, 70.f, 70.f, solid(ink(kOrange)), "accent-sq");
	rig::makeCircle(420.f, 200.f, 130.f, outlined(ink(kRed), ink(kWhite), 3.f), "demo-circle");
	rig::makeEllipse(250.f, 360.f, 260.f, 90.f, solid(ink(kViolet)), "demo-ellipse");
	rig::makeRect(40.f, 240.f, 120.f, 120.f, solid(ink(kAmber)), "accent-sq2");
	rig::makeLine(40.f, 400.f, 340.f, 40.f, pen(ink(kChalk), 0.85f, 3.f), "demo-line");
	rig::makeCircle(740.f, 70.f, 28.f, solid(ink(kPaper)), "dot");
	rig::makeCircle(100.f, 520.f, 55.f, outlined(ink(kBlue), ink(kPaper), 3.f), "echo");
	rig::makeRect(200.f, 470.f, 200.f, 90.f, solid(ink(kGreen)), "bar");
	rig::makeEllipse(680.f, 520.f, 160.f, 90.f, solid(ink(kRed)), "oval-echo");
	rig::makeRect(520.f, 470.f, 70.f, 90.f, solid(ink(kOrange)), "accent-tall");

	// Meshes are first-class data (CMesh) and present after shapes, so they may overlap.
	rig::makeMeshTriangle({520.f, 30.f, 0.f}, {780.f, 300.f, 0.f}, {420.f, 300.f, 0.f},
						  solid(ink(kYellow)), "demo-tri");
	rig::makeMeshQuad(580.f, 280.f, 180.f, 160.f, solid(ink(kGreen)), "demo-quad");
	rig::makeMeshTriangle({420.f, 460.f, 0.f}, {530.f, 580.f, 0.f}, {310.f, 580.f, 0.f},
						  solid(ink(kYellow)), "tri-echo");
}
