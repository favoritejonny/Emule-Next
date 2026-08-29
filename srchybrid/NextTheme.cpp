// Built-in colour themes for eMule Next.
// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later
// See ../LICENSE and ../CHANGES.md.
#include "stdafx.h"
#include "Preferences.h"
#include "NextTheme.h"

bool CNextTheme::s_bLoaded = false;
EEmuleNextTheme CNextTheme::s_eTheme = entModernLight;

CString CNextTheme::GetSettingsPath()
{
	return thePrefs.GetMuleDirectory(EMULE_CONFIGDIR) + _T("eMuleNextTheme.ini");
}

void CNextTheme::Load()
{
	if (s_bLoaded)
		return;

	s_bLoaded = true;
	const int iTheme = ::GetPrivateProfileInt(_T("Interface"), _T("Theme"), entModernLight, GetSettingsPath());
	if (iTheme >= entModernLight && iTheme <= entClassic)
		s_eTheme = static_cast<EEmuleNextTheme>(iTheme);
}

EEmuleNextTheme CNextTheme::GetTheme()
{
	Load();
	return s_eTheme;
}

void CNextTheme::SetTheme(EEmuleNextTheme theme)
{
	Load();
	s_eTheme = (theme >= entModernLight && theme <= entClassic) ? theme : entModernLight;

	CString strTheme;
	strTheme.Format(_T("%d"), static_cast<int>(s_eTheme));
	::WritePrivateProfileString(_T("Interface"), _T("Theme"), strTheme, GetSettingsPath());
}
