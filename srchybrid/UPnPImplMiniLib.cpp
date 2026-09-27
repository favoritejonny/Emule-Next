//this file is part of eMule
//Copyright (C)2002-2026 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / https://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "StdAfx.h"
#include "preferences.h"
#include "UPnPImplMiniLib.h"
#include "Log.h"
#include "Otherfunctions.h"
#include "miniupnpc\include\miniupnpc.h"
#include "miniupnpc\include\upnpcommands.h"
#include "opcodes.h"
#include "ConnectionSetupPolicy.h"
#include "Resource.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMutex CUPnPImplMiniLib::m_mutBusy;

static LPCSTR const sTCPa = "TCP";
static LPCSTR const sUDPa = "UDP";
static LPCTSTR const sTCP = _T("TCP");
static LPCTSTR const sUDP = _T("UDP");
static const char sMappingTCP[] = "eMule Next TCP";
static const char sMappingUDP[] = "eMule Next UDP";

CUPnPImplMiniLib::CUPnPImplMiniLib()
	: m_pURLs()
	, m_pIGDData()
	, m_hThreadHandle()
	, m_bSucceededOnce()
	, m_bAbortDiscovery()
{
	m_nOldUDPPort = 0;
	m_nOldTCPPort = 0;
	m_nOldTCPWebPort = 0;
	m_achLanIP[0] = 0;
	m_achWanIP[0] = 0;
}

CUPnPImplMiniLib::~CUPnPImplMiniLib()
{
	Cleanup();
}

bool CUPnPImplMiniLib::IsReady()
{
	if (m_bAbortDiscovery)
		return false;
	// the only check we need to do is if we are already busy with some async/threaded function
	CSingleLock lockTest(&m_mutBusy);
	return lockTest.Lock(0);
}

void CUPnPImplMiniLib::StopAsyncFind()
{
	if (m_hThreadHandle != NULL) {
		m_bAbortDiscovery = true;	// if there is a thread, tell it to abort as soon as possible - he won't sent a Result message when aborted
		CSingleLock lockTest(&m_mutBusy);
		if (!lockTest.Lock(SEC2MS(7))) {	// give the thread 7 seconds to exit gracefully - it should never really take that long
			// that is quite bad, something seems to be locked up. There isn't a good solution here, we need the thread to quit
			// or it might try to access the object later, but terminating is quite bad too. Well.
			DebugLogError(_T("Waiting for UPnP StartDiscoveryThread to quit failed, trying to terminate the thread..."));

			if (m_hThreadHandle != NULL)
				DebugLogError(::TerminateThread(m_hThreadHandle, 0) ? _T("...OK") : _T("...Failed"));
			else
				ASSERT(0);
		} else
			DebugLog(_T("Aborted any possible UPnP StartDiscoveryThread"));
		m_hThreadHandle = NULL;
	}
	m_bAbortDiscovery = false;
}

void CUPnPImplMiniLib::DeletePorts()
{
	GetOldPorts();
	m_nUDPPort = 0;
	m_nTCPPort = 0;
	m_nTCPWebPort = 0;
	m_bUPnPPortsForwarded = TRIS_FALSE;
	DeletePorts(false);
}

void CUPnPImplMiniLib::DeletePort(uint16 port, LPCTSTR prot)
{
	if (port != 0) {
		char achPort[8];
		snprintf(achPort, _countof(achPort), "%hu", port);
		char actualIP[16] = {}, actualPort[6] = {}, description[80] = {}, enabled[4] = {};
		const char* protocol = _tcscmp(prot, sTCP) == 0 ? sTCPa : sUDPa;
		const char* ownDescription = _tcscmp(prot, sTCP) == 0 ? sMappingTCP : sMappingUDP;
		const int query = UPNP_GetSpecificPortMappingEntry(m_pURLs->controlURL,
			m_pIGDData->first.servicetype, achPort, protocol, NULL,
			actualIP, actualPort, description, enabled, NULL);
		if (query != UPNPCOMMAND_SUCCESS || strcmp(description, ownDescription) != 0
			|| !ConnectionSetupPolicy::MappingIsActive(actualIP, actualPort, enabled, m_achLanIP, port))
		{
			DebugLog(_T("Not deleting unowned or changed mapping for %s port %hu"), prot, port);
			return;
		}
		int nResult = UPNP_DeletePortMapping(m_pURLs->controlURL, m_pIGDData->first.servicetype, achPort, CStringA(prot), NULL);
		if (nResult == UPNPCOMMAND_SUCCESS)
			DebugLog(_T("Successfully removed mapping for %s port %hu"), prot, port);
		else
			DebugLogWarning(_T("Failed to remove mapping for %s port %hu"), prot, port);
	}
}

void CUPnPImplMiniLib::GetOldPorts()
{
	// Also clean up partial attempts (for example TCP succeeded but UDP
	// failed). DeletePort verifies both ownership and destination first.
	if (m_pURLs != NULL && m_pIGDData != NULL) {
		m_nOldUDPPort = m_nUDPPort;
		m_nOldTCPPort = m_nTCPPort;
		m_nOldTCPWebPort = m_nTCPWebPort;
	} else {
		m_nOldUDPPort = 0;
		m_nOldTCPPort = 0;
		m_nOldTCPWebPort = 0;
	}
}

void CUPnPImplMiniLib::DeletePorts(bool bSkipLock)
{
	// this function can be blocking when called when eMule exits, and we need to wait for it to finish
	// before going on anyway. It might be called from the non-blocking StartDiscovery() function too however
	CSingleLock lockTest(&m_mutBusy);
	if (bSkipLock || lockTest.Lock(0)) {
		if (m_pURLs == NULL || m_pURLs->controlURL == NULL || m_pIGDData == NULL)
			ASSERT(!thePrefs.IsUPnPEnabled());
		else {
			DeletePort(m_nOldTCPPort, sTCP);
			DeletePort(m_nOldUDPPort, sUDP);
			DeletePort(m_nOldTCPWebPort, sTCP);
		}
		m_nOldTCPPort = 0;
		m_nOldUDPPort = 0;
		m_nOldTCPWebPort = 0;
	} else
		DebugLogError(_T("Unable to remove port mappings - implementation still busy"));
}

void CUPnPImplMiniLib::StartDiscovery(uint16 nTCPPort, uint16 nUDPPort, uint16 nTCPWebPort)
{
	DebugLog(_T("Using MiniUPnPLib based implementation"));
	DebugLog(_T("miniupnpc (c) 2005-2026 Thomas Bernard - http://miniupnp.free.fr/"));
	GetOldPorts();
	m_nUDPPort = nUDPPort;
	m_nTCPPort = nTCPPort;
	m_nTCPWebPort = nTCPWebPort;
	m_bUPnPPortsForwarded = TRIS_UNKNOWN;
	m_bCheckAndRefresh = false;
	m_diagnosticMessageID = 0;

	Cleanup();
	if (!m_bAbortDiscovery)
		StartThread();
}

bool CUPnPImplMiniLib::CheckAndRefresh()
{
	// in CheckAndRefresh we don't do any new time consuming discovery tries, we expect to find the same router like the first time
	// and of course we also don't delete old ports (this was done in Discovery) but only check that our current mappings still exist
	// and refresh them if not
	if (m_bAbortDiscovery || !m_bSucceededOnce || m_pURLs == NULL || m_pIGDData == NULL
	    || m_pURLs->controlURL == NULL || m_nTCPPort == 0)
	{
		DebugLog(_T("Not refreshing UPnP ports because they don't seem to be forwarded in the first place"));
		return false;
	}
//>>> WiZaRd
	if (!IsReady()) {
		DebugLog(_T("Not refreshing UPnP ports because they are already in the process of being refreshed"));
		return false;
	}
//<<< WiZaRd

	DebugLog(_T("Checking and refreshing UPnP ports"));
	m_bCheckAndRefresh = true;
	m_diagnosticMessageID = 0;
	StartThread();
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// CUPnPImplMiniLib::CStartDiscoveryThread Implementation
typedef CUPnPImplMiniLib::CStartDiscoveryThread CStartDiscoveryThread;
IMPLEMENT_DYNCREATE(CStartDiscoveryThread, CWinThread)

CUPnPImplMiniLib::CStartDiscoveryThread::CStartDiscoveryThread()
	: m_pOwner()
{
}

BOOL CUPnPImplMiniLib::CStartDiscoveryThread::InitInstance()
{
	InitThreadLocale();
	return TRUE;
}

int CUPnPImplMiniLib::CStartDiscoveryThread::Run()
{
	DbgSetThreadName("CUPnPImplMiniLib::CStartDiscoveryThread");
	if (!m_pOwner)
		return 0;

	CSingleLock sLock(&m_pOwner->m_mutBusy);
	if (!sLock.Lock(0)) {
		DebugLogWarning(_T("CUPnPImplMiniLib::CStartDiscoveryThread::Run, failed to acquire Lock, another Mapping try might be running already"));
		return 0;
	}

	if (m_pOwner->m_bAbortDiscovery)// requesting to abort ASAP?
		return 0;

	bool bSucceeded = false;
#if !(defined(_DEBUG) || defined(_BETA) || defined(_DEVBUILD))
	try
#endif
	{
		if (!m_pOwner->m_bCheckAndRefresh) {
			int error = 0;
			UPNPDev *structDeviceList = upnpDiscover(2000, thePrefs.GetBindAddrA(), NULL, 0, 0, 2, &error);
			if (structDeviceList == NULL) {
				DebugLog(_T("UPNP: No Internet Gateway Devices found, aborting: %d"), error);
				m_pOwner->m_bUPnPPortsForwarded = TRIS_FALSE;
				m_pOwner->SendResultMessage();
				return 0;
			}

			if (m_pOwner->m_bAbortDiscovery) {	// requesting to abort ASAP?
				freeUPNPDevlist(structDeviceList);
				return 0;
			}

			DebugLog(_T("List of UPNP devices found on the network:"));
			for (UPNPDev *pDevice = structDeviceList; pDevice != NULL; pDevice = pDevice->pNext)
				DebugLog(_T("Desc: %S, st: %S"), pDevice->descURL, pDevice->st);

			m_pOwner->m_pURLs = new UPNPUrls();
			m_pOwner->m_pIGDData = new IGDdatas();
			*m_pOwner->m_achLanIP = 0;
			*m_pOwner->m_achWanIP = 0;
			int iResult = UPNP_GetValidIGD(structDeviceList, m_pOwner->m_pURLs, m_pOwner->m_pIGDData
							, m_pOwner->m_achLanIP, sizeof m_pOwner->m_achLanIP
							, m_pOwner->m_achWanIP, sizeof m_pOwner->m_achWanIP);
			freeUPNPDevlist(structDeviceList);
			bool bNotFound = false;
			switch (iResult) {
			case 1:
				DebugLog(_T("Found valid IGD : %S"), m_pOwner->m_pURLs->controlURL);
				break;
			case 2:
				DebugLog(_T("Found an IGD with a reserved IP address (%S) : %S"), m_pOwner->m_achWanIP, m_pOwner->m_pURLs->controlURL);
				m_pOwner->m_diagnosticMessageID = IDS_CONNSETUP_SHARED;
				bNotFound = true;
				break;
			case 3:
				DebugLog(_T("Found a (not connected?) IGD : %S - Trying to continue anyway"), m_pOwner->m_pURLs->controlURL);
				break;
			case 4:
				DebugLog(_T("UPnP device found. Is it an IGD? : %S - Trying to continue anyway"), m_pOwner->m_pURLs->controlURL);
				break;
			default:
				DebugLog(_T("Found device (IGD?) : %S - Aborting"), m_pOwner->m_pURLs->controlURL != NULL ? m_pOwner->m_pURLs->controlURL : "(none)");
				bNotFound = true;
			}
			if (bNotFound || m_pOwner->m_pURLs->controlURL == NULL) {
				m_pOwner->m_bUPnPPortsForwarded = TRIS_FALSE;
				m_pOwner->SendResultMessage();
				return 0;
			}
			DebugLog(_T("Our LAN IP: %S"), m_pOwner->m_achLanIP);

			if (m_pOwner->m_bAbortDiscovery)// requesting to abort ASAP?
				return 0;

			// do we still have old mappings? Remove them first
			m_pOwner->DeletePorts(true);
		}

		bSucceeded = OpenPort(m_pOwner->m_nTCPPort, true, m_pOwner->m_achLanIP, m_pOwner->m_bCheckAndRefresh);
		if (bSucceeded && m_pOwner->m_nUDPPort != 0)
			bSucceeded = OpenPort(m_pOwner->m_nUDPPort, false, m_pOwner->m_achLanIP, m_pOwner->m_bCheckAndRefresh);
		if (bSucceeded) {
			if (m_pOwner->m_nOldTCPWebPort)
				m_pOwner->DeletePort(m_pOwner->m_nOldTCPWebPort, sTCP);	//unmap WebServer port (late binding)
			if (m_pOwner->m_nTCPWebPort)
				OpenPort(m_pOwner->m_nTCPWebPort, true, m_pOwner->m_achLanIP, m_pOwner->m_bCheckAndRefresh);	// don't fail if only the Web Interface port fails for some reason
		}
#if !(defined(_DEBUG) || defined(_BETA) || defined(_DEVBUILD))
	} catch (...) {
		DebugLogError(_T("Unknown Exception in CUPnPImplMiniLib::CStartDiscoveryThread::Run()"));
#endif
	}
	if (!m_pOwner->m_bAbortDiscovery) {	// don't send the result on an abort request
		if (bSucceeded) {
			m_pOwner->m_bUPnPPortsForwarded = TRIS_TRUE;
			m_pOwner->m_bSucceededOnce = true;
		} else
			m_pOwner->m_bUPnPPortsForwarded = TRIS_FALSE;
		m_pOwner->SendResultMessage();
	}
	return 0;
}

bool CUPnPImplMiniLib::CStartDiscoveryThread::OpenPort(uint16 nPort, bool bTCP, char *pachLANIP, bool bCheckAndRefresh)
{
	if (m_pOwner->m_bAbortDiscovery)
		return false;

	char achPort[8];
	snprintf(achPort, _countof(achPort), "%hu", nPort);

	// Query on every attempt, including the first one. An existing mapping
	// must point to this exact endpoint and be enabled; never overwrite a
	// different PC's rule or a rule which the user deliberately disabled.
	char achOutIP[20] = {};
	char achOutPort[8] = {};
	char enabled[4] = {};
	int nResult = UPNP_GetSpecificPortMappingEntry(m_pOwner->m_pURLs->controlURL, m_pOwner->m_pIGDData->first.servicetype
												 , achPort
												 , (bTCP ? sTCPa : sUDPa)
												 , NULL
												 , achOutIP, achOutPort
												 , NULL, enabled, NULL);

	if (nResult == UPNPCOMMAND_SUCCESS) {
		if (ConnectionSetupPolicy::MappingIsActive(achOutIP, achOutPort, enabled, pachLANIP, nPort)) {
			DebugLog(_T("Checking UPnP: Mapping for port %hu (%s) on local IP %S still exists"), nPort, (bTCP ? sTCP : sUDP), achOutIP);
			return true;
		}
		m_pOwner->m_diagnosticMessageID = IDS_CONNSETUP_CONFLICT;
		return false;
	}
	// Only NoSuchEntryInArray authorizes adding a new mapping. An unreadable
	// router table is not evidence that the requested port is free.
	if (nResult != 714)
		return false;
	if (bCheckAndRefresh)
		DebugLogWarning(_T("UPnP mapping for port %hu is gone, requesting it again"), nPort);

	nResult = UPNP_AddPortMapping(m_pOwner->m_pURLs->controlURL
								, m_pOwner->m_pIGDData->first.servicetype
								, achPort, achPort, pachLANIP
								, (bTCP ? sMappingTCP : sMappingUDP)
								, (bTCP ? sTCPa : sUDPa)
								, NULL, NULL);

	if (nResult != UPNPCOMMAND_SUCCESS) {
		DebugLog(_T("Adding PortMapping failed, Error Code %u"), nResult);
		return false;
	}

	if (m_pOwner->m_bAbortDiscovery)
		return false;

	// make sure it really worked
	achOutIP[0] = 0;
	achOutPort[0] = 0;
	enabled[0] = 0;
	nResult = UPNP_GetSpecificPortMappingEntry(m_pOwner->m_pURLs->controlURL
											 , m_pOwner->m_pIGDData->first.servicetype
											 , achPort
											 , (bTCP ? sTCPa : sUDPa)
											 , NULL
											 , achOutIP, achOutPort
											 , NULL, enabled, NULL);

	if (nResult == UPNPCOMMAND_SUCCESS
		&& ConnectionSetupPolicy::MappingIsActive(achOutIP, achOutPort, enabled, pachLANIP, nPort))
	{
		DebugLog(_T("Successfully added mapping for port %hu (%s) on local IP %S"), nPort, (bTCP ? sTCP : sUDP), achOutIP);
		return true;
	}

	DebugLogWarning(_T("Failed to verify mapping for port %hu (%s) on local IP %S - considering as failed"), nPort, (bTCP ? sTCP : sUDP), achOutIP);
	// maybe counting this as error is a bit harsh as this may lead to false negatives, however if we would risk false positives
	// this would mean that the fallback implementations are not tried because eMule thinks it worked out fine
	return false;
}

void CUPnPImplMiniLib::Cleanup()
{
	FreeUPNPUrls(m_pURLs);
	delete m_pURLs;
	m_pURLs = NULL;

	delete m_pIGDData;
	m_pIGDData = NULL;
}

void CUPnPImplMiniLib::StartThread()
{
	CStartDiscoveryThread *pStartDiscoveryThread = (CStartDiscoveryThread*)AfxBeginThread(RUNTIME_CLASS(CStartDiscoveryThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	m_hThreadHandle = pStartDiscoveryThread->m_hThread;
	pStartDiscoveryThread->SetValues(this);
	pStartDiscoveryThread->ResumeThread();
}
