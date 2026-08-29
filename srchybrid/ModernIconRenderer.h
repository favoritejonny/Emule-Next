// Central, DPI-aware icon renderer for the eMule Next interface.
// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later
// See ../LICENSE and ../CHANGES.md.
#pragma once

// Returns false when the resource deliberately keeps its original artwork
// (for example third-party client badges and chat smileys).
bool CreateNextModernIcon(LPCTSTR pszResourceName, int cx, int cy, HICON* phIcon);
