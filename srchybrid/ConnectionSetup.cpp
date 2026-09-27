// eMule Next - GPL-2.0-or-later
#include "stdafx.h"
#include "ConnectionSetup.h"
#include "ConnectionSetupPolicy.h"
#include "Preferences.h"
#include "Resource.h"
#include <iphlpapi.h>
#include <netfw.h>
#include <vector>

UINT GetAutomaticSetupRestriction()
{
	// A proxy or an explicit interface binding must never be bypassed by a
	// convenience feature. VPN detection is deliberately conservative, not a
	// substitute for a VPN kill switch or the user's explicit confirmation.
	const char* bindAddress = thePrefs.GetBindAddrA();
	if (thePrefs.GetProxySettings().bUseProxy || (bindAddress != nullptr && *bindAddress != 0))
		return IDS_CONNSETUP_VPN;
	return GetAutomaticNetworkRestriction();
}

UINT GetAutomaticNetworkRestriction()
{
	ULONG size = 15 * 1024;
	std::vector<unsigned char> buffer(size);
	ULONG result = ERROR_BUFFER_OVERFLOW;
	for (unsigned attempt = 0; attempt < 3 && result == ERROR_BUFFER_OVERFLOW; ++attempt) {
		if (size > 1024 * 1024)
			return IDS_CONNSETUP_UNKNOWN;
		buffer.resize(size);
		result = GetAdaptersAddresses(AF_UNSPEC,
			GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
			nullptr, reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data()), &size);
	}
	if (result != NO_ERROR)
		return IDS_CONNSETUP_UNKNOWN;

	bool activeNetwork = false;
	for (const IP_ADAPTER_ADDRESSES* adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
		adapter != nullptr; adapter = adapter->Next)
	{
		if (adapter->OperStatus != IfOperStatusUp || adapter->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
			continue;
		activeNetwork = true;
		CString name(adapter->Description != nullptr ? adapter->Description : L"");
		name += adapter->FriendlyName != nullptr ? adapter->FriendlyName : L"";
		name.MakeLower();
		if (adapter->IfType == IF_TYPE_PPP || adapter->IfType == IF_TYPE_TUNNEL
			|| name.Find(_T("vpn")) >= 0 || name.Find(_T("wireguard")) >= 0
			|| name.Find(_T("wintun")) >= 0 || name.Find(_T("tap-")) >= 0
			|| name.Find(_T("tailscale")) >= 0 || name.Find(_T("zerotier")) >= 0)
			return IDS_CONNSETUP_VPN;
	}
	if (!activeNetwork)
		return IDS_CONNSETUP_UNKNOWN;

	CComPtr<INetFwPolicy2> policy;
	long profiles = 0;
	if (FAILED(policy.CoCreateInstance(__uuidof(NetFwPolicy2)))
		|| FAILED(policy->get_CurrentProfileTypes(&profiles)) || profiles == 0)
		return IDS_CONNSETUP_UNKNOWN;
	return ConnectionSetupPolicy::AllowAutomaticSetup(true, false, true, profiles)
		? 0 : IDS_CONNSETUP_PRIVATE;
}
