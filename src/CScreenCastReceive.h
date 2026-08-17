#pragma once

#include <string>
#include <vector>

#include "CScreenCast.h"
#include "ecs/PropertyReflection.h"

namespace rigkit {
namespace ecs {

/**
 * @brief Inbound cast session — data only (UxPlay-style receive direction).
 * @details Fulfillment in **rigScreenCast** presents decoded frames into the
 * host window / GL path — never desktop capture. Advertise only protocols the
 * build can actually receive (RigKit peer; AirPlay when UxPlay is linked).
 */
struct CScreenCastReceive {
	bool enabled = false;
	/** @brief Which sender family to accept; Auto = RigKit peer path. */
	ScreenCastProtocol protocol = ScreenCastProtocol::RigKit;
	int listenPort = 7411;
	std::string serviceName = "RigKit";
	ScreenCastState state = ScreenCastState::Idle;
	std::string status;
	std::string senderName;
	std::string senderHost;
	int frameWidth = 0;
	int frameHeight = 0;
	int framesReceived = 0;

	std::vector<sProp> GetProperties() {
		static const char* const kProtocol[] = {"auto", "airplay", "googlecast", "rigkit"};
		static const char* const kState[] = {"idle",	   "discovering", "ready",
											"connecting", "streaming",	 "error"};
		return {{0, "Enabled", EPT_BOOL, &enabled},
				{1, "Protocol", EPT_ENUM, reinterpret_cast<int*>(&protocol), kProtocol, 4},
				{2, "Listen Port", EPT_INT, &listenPort},
				{3, "Service Name", EPT_STRING, &serviceName},
				{4, "State", EPT_ENUM, reinterpret_cast<int*>(&state), kState, 6},
				{5, "Status", EPT_STRING, &status},
				{6, "Sender Name", EPT_STRING, &senderName},
				{7, "Sender Host", EPT_STRING, &senderHost},
				{8, "Frame Width", EPT_INT, &frameWidth},
				{9, "Frame Height", EPT_INT, &frameHeight},
				{10, "Frames Received", EPT_INT, &framesReceived}};
	}
};

} // namespace ecs
} // namespace rigkit
