#pragma once

/**
 * @file
 * @brief Screen-cast session POD and its protocol / payload / state enums.
 * @details Data only. Receiver discovery, window capture, and sockets live in
 * rigScreenCast (AirPlay / TV / host window). App-to-app AV is **rigCast**.
 */

#include <string>
#include <vector>
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/** @brief Wire / receiver family for a screen-cast session. */
enum class ScreenCastProtocol : int {
	Auto = 0,
	AirPlay = 1,
	GoogleCast = 2,
	RigKit = 3
};

/**
 * @brief Pixel source for a cast session.
 * @details Host window framebuffer only - never desktop / monitor capture.
 */
enum class ScreenCastSource : int { HostWindow = 0 };

/** @brief What the session pushes to the receiver. */
enum class ScreenCastPayload : int {
	LiveWindow = 0,
	ImageFile = 1
};

/** @brief Session lifecycle - written by fulfillment, readable in Properties. */
enum class ScreenCastState : int {
	Idle = 0,
	Discovering = 1,
	Ready = 2,
	Connecting = 3,
	Streaming = 4,
	Error = 5
};

/**
 * @brief Screen-cast session intent + selected target - data only.
 * @details Protocol-neutral (AirPlay, Google Cast, RigKit peer). Capture and
 * sockets live in **rigScreenCast**. `source` is always HostWindow. For
 * `ImageFile`, set `imagePath` or compose `CAssetRef` (Kind::Image) on the
 * same entity.
 */
struct CScreenCast {
	bool enabled = false;
	ScreenCastProtocol protocol = ScreenCastProtocol::Auto;
	ScreenCastSource source = ScreenCastSource::HostWindow;
	ScreenCastPayload payload = ScreenCastPayload::LiveWindow;
	std::string targetName;
	std::string targetHost;
	int targetPort = 0;
	std::string targetId;
	std::string pairPin;
	std::string imagePath;
	ScreenCastState state = ScreenCastState::Idle;
	std::string status;

	std::vector<sProp> GetProperties() {
		static const char* const kProtocol[] = {"auto", "airplay", "googlecast", "rigkit"};
		static const char* const kSource[] = {"hostWindow"};
		static const char* const kPayload[] = {"liveWindow", "imageFile"};
		static const char* const kState[] = {"idle",	   "discovering", "ready",
											"connecting", "streaming",	 "error"};
		return {{0, "Enabled", EPT_BOOL, &enabled},
				{1, "Protocol", EPT_ENUM, reinterpret_cast<int*>(&protocol), kProtocol, 4},
				{2, "Source", EPT_ENUM, reinterpret_cast<int*>(&source), kSource, 1},
				{3, "Payload", EPT_ENUM, reinterpret_cast<int*>(&payload), kPayload, 2},
				{4, "Target Name", EPT_STRING, &targetName},
				{5, "Target Host", EPT_STRING, &targetHost},
				{6, "Target Port", EPT_INT, &targetPort},
				{7, "Target Id", EPT_STRING, &targetId},
				{8, "Pair PIN", EPT_STRING, &pairPin},
				{9, "Image Path", EPT_STRING, &imagePath},
				{10, "State", EPT_ENUM, reinterpret_cast<int*>(&state), kState, 6},
				{11, "Status", EPT_STRING, &status}};
	}
};

} // namespace ecs
} // namespace rigkit
