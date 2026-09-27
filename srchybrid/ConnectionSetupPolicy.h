// eMule Next - GPL-2.0-or-later
#pragma once
#include <cstring>

namespace ConnectionSetupPolicy
{

inline bool ValidPorts(unsigned tcp, unsigned udp, bool udpDisabled)
{
	return tcp > 0 && tcp <= 65535 && (udpDisabled || (udp > 0 && udp <= 65535));
}

inline bool MatchesEndpoint(const char* ip, const char* port, const char* expectedIP, unsigned expectedPort)
{
	if (ip == nullptr || port == nullptr || expectedIP == nullptr || *ip == 0
		|| std::strcmp(ip, expectedIP) != 0 || expectedPort == 0 || expectedPort > 65535)
		return false;
	unsigned value = 0;
	unsigned digits = 0;
	for (; *port != 0; ++port) {
		if (++digits > 5 || *port < '0' || *port > '9')
			return false;
		value = value * 10 + static_cast<unsigned>(*port - '0');
	}
	return digits != 0 && value == expectedPort;
}

inline bool MappingIsActive(const char* ip, const char* port, const char* enabled,
	const char* expectedIP, unsigned expectedPort)
{
	return enabled != nullptr && std::strcmp(enabled, "1") == 0
		&& MatchesEndpoint(ip, port, expectedIP, expectedPort);
}

// Windows profile bits: private=2. Mixed, public, domain and unknown networks
// require manual configuration; no network profile is changed by the client.
inline bool AllowAutomaticSetup(bool consent, bool possibleVPN, bool profileKnown, long profiles)
{
	return consent && !possibleVPN && profileKnown && profiles == 2;
}

// The normal startup auto-connect check may run while the modal first-run
// wizard is still open. Connect on Finish only when that check has already
// passed; otherwise startup will read the newly saved preference itself.
inline bool ShouldConnectAfterFirstRunFinish(bool firstRun, bool autoConnect, bool applicationRunning)
{
	return firstRun && autoConnect && applicationRunning;
}

}
