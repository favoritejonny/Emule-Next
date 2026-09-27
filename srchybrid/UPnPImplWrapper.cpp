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
#include "UPnPImplWrapper.h"
#include "UPnPImpl.h"
#include "UPnPImplWinServ.h"
#include "UPnPImplMiniLib.h"
#include "UPnPImplPcpNatPmp.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CUPnPImplWrapper::CUPnPImplWrapper()
{
	if (!thePrefs.IsWinServUPnPImplDisabled())
		m_liAvailable.AddTail(new CUPnPImplWinServ());
	if (!thePrefs.IsMinilibUPnPImplDisabled())
		m_liAvailable.AddTail(new CUPnPImplMiniLib());
	if (m_liAvailable.IsEmpty())
		m_liAvailable.AddTail(new CUPnPImplNone());
	m_liAvailable.AddTail(new CUPnPImplPcpNatPmp());
	Init();
}

CUPnPImplWrapper::~CUPnPImplWrapper()
{
	while (!m_liAvailable.IsEmpty())
		delete m_liAvailable.RemoveHead();
	while (!m_liUsed.IsEmpty())
		delete m_liUsed.RemoveHead();
	m_pActiveImpl = NULL;
}

void CUPnPImplWrapper::Init()
{
	ASSERT(!m_liAvailable.IsEmpty());
	m_pActiveImpl = NULL;

	for (POSITION pos = m_liAvailable.GetHeadPosition(); pos != NULL;) {
		POSITION pos2 = pos;
		CUPnPImpl *tmp = m_liAvailable.GetNext(pos);
		if (tmp->GetImplementationID() == thePrefs.GetLastWorkingUPnPImpl()
			&& (tmp->GetImplementationID() != UPNP_IMPL_PCP_NATPMP || thePrefs.IsUPnPHomeOnly())) {
			m_pActiveImpl = tmp;
			m_liAvailable.RemoveAt(pos2);
			break;
		}
	}

	if (m_pActiveImpl == NULL) {
		// Do not silently enable the new protocols for legacy UPnP users.
		for (POSITION pos = m_liAvailable.GetHeadPosition(); pos != NULL;) {
			POSITION current = pos;
			CUPnPImpl* candidate = m_liAvailable.GetNext(pos);
			if (candidate->GetImplementationID() != UPNP_IMPL_PCP_NATPMP) {
				m_pActiveImpl = candidate;
				m_liAvailable.RemoveAt(current);
				break;
			}
		}
	}
	m_liUsed.AddTail(m_pActiveImpl);
}

void CUPnPImplWrapper::Reset()
{
	while (!m_liUsed.IsEmpty())
		m_liAvailable.AddTail(m_liUsed.RemoveHead());
	Init();
}

bool CUPnPImplWrapper::SwitchImplentation()
{
	for (POSITION pos = m_liAvailable.GetHeadPosition(); pos != NULL;) {
		POSITION current = pos;
		CUPnPImpl* candidate = m_liAvailable.GetNext(pos);
		if (candidate->GetImplementationID() == UPNP_IMPL_PCP_NATPMP) continue;
		m_liAvailable.RemoveAt(current);
		m_pActiveImpl = candidate;
		m_liUsed.AddTail(m_pActiveImpl);
		return true;
	}
	return false;
}

bool CUPnPImplWrapper::SelectImplementation(int implementationID)
{
	if (m_pActiveImpl->GetImplementationID() == implementationID)
		return true;
	for (POSITION pos = m_liAvailable.GetHeadPosition(); pos != NULL;) {
		POSITION current = pos;
		CUPnPImpl* implementation = m_liAvailable.GetNext(pos);
		if (implementation->GetImplementationID() == implementationID) {
			m_liAvailable.RemoveAt(current);
			m_liUsed.AddTail(implementation);
			m_pActiveImpl = implementation;
			return true;
		}
	}
	return false;
}
