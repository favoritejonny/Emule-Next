// Built-in colour themes for eMule Next. Kept separate from legacy skin files
// so the theme works in normal and portable installations without assets.
// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later
// See ../LICENSE and ../CHANGES.md.
#pragma once

enum EEmuleNextTheme
{
	entModernLight = 0,
	entModernAurora,
	entClassic
};

class CNextTheme
{
public:
	static EEmuleNextTheme GetTheme();
	static void SetTheme(EEmuleNextTheme theme);

private:
	static void Load();
	static CString GetSettingsPath();

	static bool s_bLoaded;
	static EEmuleNextTheme s_eTheme;
};
