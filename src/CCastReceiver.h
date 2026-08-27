#pragma once

#include <string>
#include <vector>

#include "ecs/PropertyReflection.h"
#include "CScreenCast.h"

namespace rigkit {
namespace ecs {

/**
 * @brief One discovered cast receiver — data only.
 * @details Fulfillment (**rigScreenCast**) writes rows from mDNS; no sockets
 * here. Prefer one entity per receiver.
 */
struct CCastReceiver {
	std::string name;
	std::string host;
	int port = 0;
	ScreenCastProtocol protocol = ScreenCastProtocol::Auto;
	std::string serviceId;

	std::vector<sProp> GetProperties() {
		static const char* const kProtocol[] = {"auto", "airplay", "googlecast", "rigkit"};
		return {{0, "Name", EPT_STRING, &name},
				{1, "Host", EPT_STRING, &host},
				{2, "Port", EPT_INT, &port},
				{3, "Protocol", EPT_ENUM, reinterpret_cast<int*>(&protocol), kProtocol, 4},
				{4, "Service Id", EPT_STRING, &serviceId}};
	}
};

} // namespace ecs
} // namespace rigkit
