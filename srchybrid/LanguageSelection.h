// eMule Next - GPL-2.0-or-later
#pragma once
#include "langids.h"

namespace LanguageSelection
{

// Match regional Windows UI variants to translations shipped with the client.
// Keep Chinese scripts distinct; do not fall back to a different script.
inline LANGID RegionalFallback(LANGID windowsLanguage)
{
	switch (PRIMARYLANGID(windowsLanguage)) {
	case LANG_NEUTRAL:
		return 0;
	case LANG_ARABIC:
		return LANGID_AR_AE;
	case LANG_CHINESE:
		switch (SUBLANGID(windowsLanguage)) {
		case SUBLANG_NEUTRAL: // zh-Hans
		case SUBLANG_CHINESE_SIMPLIFIED:
		case SUBLANG_CHINESE_SINGAPORE:
			return LANGID_ZH_CN;
		case SUBLANG_CHINESE_TRADITIONAL:
		case SUBLANG_CHINESE_HONGKONG:
		case SUBLANG_CHINESE_MACAU:
		case 0x1f: // zh-Hant
			return LANGID_ZH_TW;
		default:
			return 0;
		}
	case LANG_PORTUGUESE:
		return LANGID_PT_PT;
	case LANG_NORWEGIAN:
		return SUBLANGID(windowsLanguage) == SUBLANG_NORWEGIAN_NYNORSK ? LANGID_NN_NO : LANGID_NB_NO;
	case LANG_CATALAN:
		// Windows ca-ES-valencia uses 0x0803; the legacy resource uses 0x0902.
		return SUBLANGID(windowsLanguage) == 2 ? LANGID_VA_ES : LANGID_CA_ES;
	default:
		return MAKELANGID(PRIMARYLANGID(windowsLanguage), SUBLANG_DEFAULT);
	}
}

template <typename LoadLanguage>
LANGID Resolve(LANGID savedLanguage, LANGID windowsLanguage, LoadLanguage load)
{
	// A saved choice (including English) always takes priority over Windows.
	if (savedLanguage != 0) {
		if (load(savedLanguage))
			return savedLanguage;
	} else if (windowsLanguage != 0) {
		if (load(windowsLanguage))
			return windowsLanguage;
		const LANGID fallback = RegionalFallback(windowsLanguage);
		if (fallback != 0 && fallback != windowsLanguage && load(fallback))
			return fallback;
	}

	// English is embedded: missing, incompatible or unreadable translations
	// must not block startup or silently choose an unrelated Windows language.
	load(LANGID_EN_US);
	return LANGID_EN_US;
}

}
