#pragma once

#include <cstdint>
#include <entt/entt.hpp>
#include <string>
#include <vector>

#include "CIndexedAtlas.h"
#include "CLayer.h"
#include "CSpriteSheet.h"

namespace rigkit {
class MEcs;
class rigProject;
} // namespace rigkit

namespace pixel {

enum class Tool { Pencil, Eraser, Fill };
enum class BrushShape { Square, Circle };

struct ToolState {
	Tool tool = Tool::Pencil;
	BrushShape shape = BrushShape::Square;
	int size = 1;
	int color = 8;
};

entt::entity findSheet(rigkit::MEcs& ecs);
entt::entity findPalette(rigkit::MEcs& ecs);
entt::entity findProject(rigkit::MEcs& ecs);
std::vector<entt::entity> layersByOrder(rigkit::MEcs& ecs);
entt::entity selectedLayer(rigkit::MEcs& ecs);

void registerSerializers(rigkit::rigProject& doc);
void newDocument(rigkit::MEcs& ecs, const CSpriteSheet& spec, bool stampDemo);
entt::entity addLayer(rigkit::MEcs& ecs, const std::string& name);
void deleteSelectedLayer(rigkit::MEcs& ecs);
void ensureAfterLoad(rigkit::MEcs& ecs);
void markDirty(rigkit::MEcs& ecs);

void paint(rigkit::MEcs& ecs, int x, int y, const ToolState& tool);

struct LayerSnap {
	std::string name;
	rigkit::ecs::CLayer layer;
	rigkit::ecs::CIndexedAtlas atlas;
};

bool captureLayer(rigkit::MEcs& ecs, entt::entity e, LayerSnap& out);
entt::entity restoreLayer(rigkit::MEcs& ecs, const LayerSnap& snap);
void destroyLayer(rigkit::MEcs& ecs, entt::entity e);
void applyAtlas(rigkit::MEcs& ecs, entt::entity e, const std::string& name, int w, int h,
				const std::vector<uint8_t>& indices);

void compositeSheet(rigkit::MEcs& ecs, std::vector<uint8_t>& rgba, int& w, int& h);
void compositeFrame(rigkit::MEcs& ecs, int frame, std::vector<uint8_t>& rgba, int& w, int& h);
bool exportImage(const std::string& path, const uint8_t* rgba, int w, int h);

void tickPlayback(rigkit::MEcs& ecs, float dt);

} // namespace pixel
