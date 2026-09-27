# Automatic startup language

Local prototype, updated 2 September 2026. Applies to Win32 and x64, including portable
packages. This does not change the published alpha release.

## Selection order

1. Load the language already saved in `preferences.ini`. Existing users keep
   their choice, even when Windows uses a different language or the portable
   folder is moved to another computer.
2. When no language is saved (`Language` is absent or zero), read the current
   user's Windows display language with `GetUserDefaultUILanguage`. Regional
   date/number formats and the keyboard layout do not select the UI language.
3. Try the exact translation, then the corresponding supported regional
   variant: for example Italian (Switzerland) uses Italian, German (Austria)
   uses German, and Spanish (Mexico) uses Spanish. Chinese simplified and
   traditional scripts and the two Norwegian translations remain distinct.
4. Fall back to the embedded English resources if the translation cannot be
   loaded. An unavailable saved translation also falls back to English,
   without choosing another language from Windows or blocking startup.

The chosen language is applied before the main window and the first-run
wizard are constructed, and is saved by the existing preference-saving
mechanism. There is no extra language dialog or automatic language download.
Users can still change it under Options > General. No other preference is
changed by language selection. The 30 connection-setup and guided-page messages are now
present in all 43 shipped language modules. They are a machine-assisted first
translation pass: native-speaker review remains welcome before the Beta.
Individual missing strings continue to use the existing English fallback as a
safety net.

Windows UI languages without a usable legacy language identifier or without
a shipped translation use English. This prototype does not change the
supported Windows baseline.

## Verification

The complete offline suite and Release builds passed for Win32 and x64 on
2 September 2026 with Visual Studio 2026/v145. These changes are local and the
hosted GitHub workflow has not been run for them.

The offline suite covers regional variants, explicit saved choices (including
English), unsupported/missing translations, unknown Windows identifiers and
Chinese-script fallback. It exercises the production loader after changing
only the test process's regional locale, restores it afterwards, and verifies
that the Windows UI language remains the selection source.

A second isolated portable startup loads `tests/fixtures/saved-language.ini`.
It checks the saved French choice, nickname, TCP/UDP ports and disabled UPnP,
and verifies that the fixture's SHA-256 is unchanged. Neither test opens
transfer listeners, modifies Windows language settings or discovers routers.
The source checks require the same 30 messages, valid placeholders and UTF-8
encoding in every language module. The binary self-test also loads all 43
external language DLLs (plus embedded English) and measures the visible texts
against the actual wizard controls. A native-speaker visual confirmation is
still useful during Beta testing.

## API reference

Microsoft documents the display-language API and its fallback/legacy-ID
limitations in [GetUserDefaultUILanguage](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getuserdefaultuilanguage).
The prior [GetThreadLocale](https://learn.microsoft.com/en-us/windows/win32/api/winnls/nf-winnls-getthreadlocale)
instead reflects the calling thread's regional formats.
