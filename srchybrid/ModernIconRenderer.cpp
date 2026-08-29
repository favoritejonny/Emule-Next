// This file is part of eMule Next.
// Copyright (C) 2026 Jonny Favorite
// SPDX-License-Identifier: GPL-2.0-or-later
// See ../LICENSE and ../CHANGES.md.
#include "stdafx.h"
#include "ModernIconRenderer.h"

#include <gdiplus.h>

namespace
{
enum ENextIconSymbol
{
	enisUnknown,
	enisConnect,
	enisDisconnect,
	enisNetwork,
	enisServer,
	enisSearch,
	enisTransfer,
	enisUpload,
	enisDownload,
	enisShare,
	enisChat,
	enisStatistics,
	enisSettings,
	enisTools,
	enisHelp,
	enisUser,
	enisUsers,
	enisFile,
	enisFolder,
	enisPlay,
	enisPause,
	enisStop,
	enisAdd,
	enisRemove,
	enisCopy,
	enisDelete,
	enisStar,
	enisSecurity,
	enisClock,
	enisWeb,
	enisBell,
	enisInfo,
	enisLink,
	enisMail,
	enisDisk,
	enisPreview,
	enisList,
	enisEdit,
	enisPriority,
	enisCategory,
	enisWarning,
	enisBack,
	enisForward,
	enisFilter,
	enisUserAdd,
	enisUserRemove,
	enisUserMessage,
	enisUserFiles,
	enisFileAudio,
	enisFileVideo,
	enisFilePicture,
	enisFileArchive,
	enisFileDisc,
	enisFileProgram,
	enisServerSearch,
	enisServerAdd,
	enisServerUpdate,
	enisKadDisconnected,
	enisKadConnecting,
	enisKadFirewalled,
	enisKadConnected,
	enisMuleHead,
	enisSpeed,
	enisFolderOpen,
	enisSortUp,
	enisSortDown,
	enisSortDoubleUp,
	enisSortDoubleDown
};

struct SNextIconSpec
{
	ENextIconSymbol symbol;
	Gdiplus::Color color;
};

static bool IsIconName(const CString& name, LPCTSTR const* names, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		if (name == names[i])
			return true;
	}
	return false;
}

static Gdiplus::Color IconColor(BYTE red, BYTE green, BYTE blue)
{
	return Gdiplus::Color(255, red, green, blue);
}

static SNextIconSpec MakeIconSpec(ENextIconSymbol symbol, BYTE red, BYTE green, BYTE blue)
{
	SNextIconSpec spec = { symbol, IconColor(red, green, blue) };
	return spec;
}

class CNextIconGdiplusRuntime
{
public:
	CNextIconGdiplusRuntime()
		: m_token()
		, m_ready(false)
	{
		Gdiplus::GdiplusStartupInput startupInput;
		m_ready = Gdiplus::GdiplusStartup(&m_token, &startupInput, NULL) == Gdiplus::Ok;
	}

	~CNextIconGdiplusRuntime()
	{
		if (m_ready)
			Gdiplus::GdiplusShutdown(m_token);
	}

	bool IsReady() const
	{
		return m_ready;
	}

private:
	ULONG_PTR m_token;
	bool m_ready;
};

static SNextIconSpec ResolveNextIcon(LPCTSTR pszResourceName)
{
	if (pszResourceName == NULL || *pszResourceName == _T('\0'))
		return MakeIconSpec(enisUnknown, 0, 0, 0);

	CString name(pszResourceName);
	name.MakeUpper();

	// Smileys describe message content, rather than application commands. They
	// intentionally keep their separate artwork and are not replaced here.
	if (name.Left(7) == _T("SMILEY_"))
		return MakeIconSpec(enisUnknown, 0, 0, 0);

	static LPCTSTR const connectNames[] = { _T("CONNECT"), _T("CONNECTDO"), _T("CONNECTEDHIGHHIGH") };
	static LPCTSTR const disconnectNames[] = { _T("DISCONNECT"), _T("CONNECTDROP"), _T("STOPCONNECTING") };
	static LPCTSTR const webNames[] = { _T("WEB"), _T("GLOBAL"), _T("PROXY"), _T("CONTENTDB") };
	static LPCTSTR const settingsNames[] = { _T("PREFERENCES"), _T("DISPLAY"), _T("TWEAK"), _T("GENERAL") };
	static LPCTSTR const toolsNames[] = { _T("TOOLS"), _T("CONVERT"), _T("SCHEDULER") };
	static LPCTSTR const helpNames[] = { _T("HELP"), _T("WIZARD") };
	static LPCTSTR const addNames[] = { _T("LISTADD"), _T("ADDSERVER"), _T("ADDFRIEND"), _T("EMULECOLLADD") };
	static LPCTSTR const removeNames[] = { _T("LISTREMOVE"), _T("DELETEFRIEND") };
	static LPCTSTR const deleteNames[] = { _T("DELETE"), _T("DELETEALL"), _T("DELETESELECTED"), _T("CLEARCOMPLETE"), _T("CANCEL") };
	static LPCTSTR const copyNames[] = { _T("COPY"), _T("COPYSELECTED"), _T("COPYVISIBLE"), _T("PASTELINK") };
	static LPCTSTR const diskNames[] = { _T("HARDDISK"), _T("RESTORE"), _T("RESTOREWINDOW") };
	static LPCTSTR const infoNames[] = { _T("INFO"), _T("FILEINFO"), _T("SERVERINFO"), _T("METADATA"), _T("MEDIAINFO") };
	static LPCTSTR const bellNames[] = { _T("NOTIFICATIONS") };
	static LPCTSTR const clockNames[] = { _T("STATSTIME"), _T("SCHEDULER"), _T("RECORDS") };

	if (IsIconName(name, connectNames, _countof(connectNames)))
		return MakeIconSpec(enisConnect, 47, 170, 121);
	if (IsIconName(name, disconnectNames, _countof(disconnectNames)))
		return MakeIconSpec(enisDisconnect, 224, 94, 105);
	if (IsIconName(name, settingsNames, _countof(settingsNames)))
		return MakeIconSpec(enisSettings, 58, 150, 208);
	if (IsIconName(name, toolsNames, _countof(toolsNames)))
		return MakeIconSpec(enisTools, 239, 153, 72);
	if (IsIconName(name, helpNames, _countof(helpNames)))
		return MakeIconSpec(enisHelp, 112, 99, 211);
	if (IsIconName(name, addNames, _countof(addNames)))
		return MakeIconSpec(enisAdd, 47, 170, 121);
	if (IsIconName(name, removeNames, _countof(removeNames)))
		return MakeIconSpec(enisRemove, 224, 94, 105);
	if (IsIconName(name, deleteNames, _countof(deleteNames)))
		return MakeIconSpec(enisDelete, 224, 94, 105);
	if (IsIconName(name, copyNames, _countof(copyNames)))
		return MakeIconSpec(enisCopy, 85, 132, 214);
	if (IsIconName(name, diskNames, _countof(diskNames)))
		return MakeIconSpec(enisDisk, 111, 122, 144);
	if (IsIconName(name, infoNames, _countof(infoNames)))
		return MakeIconSpec(enisInfo, 58, 150, 208);
	if (IsIconName(name, bellNames, _countof(bellNames)))
		return MakeIconSpec(enisBell, 145, 104, 218);
	if (IsIconName(name, clockNames, _countof(clockNames)))
		return MakeIconSpec(enisClock, 145, 104, 218);
	if (IsIconName(name, webNames, _countof(webNames)))
		return MakeIconSpec(enisWeb, 69, 167, 196);
	if (name == _T("NAVIGATEBACK"))
		return MakeIconSpec(enisBack, 85, 132, 214);
	if (name == _T("NAVIGATEFORWARD"))
		return MakeIconSpec(enisForward, 85, 132, 214);
	if (name == _T("NEXT_SORT_UP"))
		return MakeIconSpec(enisSortUp, 58, 150, 208);
	if (name == _T("NEXT_SORT_DOWN"))
		return MakeIconSpec(enisSortDown, 58, 150, 208);
	if (name == _T("NEXT_SORT_DOUBLE_UP"))
		return MakeIconSpec(enisSortDoubleUp, 58, 150, 208);
	if (name == _T("NEXT_SORT_DOUBLE_DOWN"))
		return MakeIconSpec(enisSortDoubleDown, 58, 150, 208);
	if (name.Left(6) == _T("FILTER"))
		return MakeIconSpec(enisFilter, 72, 176, 151);
	if (name == _T("USERADD"))
		return MakeIconSpec(enisUserAdd, 47, 170, 121);
	if (name == _T("USERDELETE"))
		return MakeIconSpec(enisUserRemove, 224, 94, 105);
	if (name == _T("USERMESSAGE"))
		return MakeIconSpec(enisUserMessage, 145, 104, 218);
	if (name == _T("USERFILES"))
		return MakeIconSpec(enisUserFiles, 85, 132, 214);
	if (name == _T("CLIENTSONQUEUE"))
		return MakeIconSpec(enisUsers, 85, 132, 214);
	if (name == _T("FILETYPEAUDIO"))
		return MakeIconSpec(enisFileAudio, 145, 104, 218);
	if (name == _T("FILETYPEVIDEO"))
		return MakeIconSpec(enisFileVideo, 224, 94, 105);
	if (name == _T("FILETYPEPICTURE"))
		return MakeIconSpec(enisFilePicture, 72, 176, 151);
	if (name == _T("FILETYPEARCHIVE"))
		return MakeIconSpec(enisFileArchive, 239, 153, 72);
	if (name == _T("FILETYPECDIMAGE"))
		return MakeIconSpec(enisFileDisc, 85, 132, 214);
	if (name == _T("FILETYPEPROGRAM"))
		return MakeIconSpec(enisFileProgram, 58, 150, 208);
	if (name == _T("SERVERSEARCH"))
		return MakeIconSpec(enisServerSearch, 69, 167, 196);
	if (name == _T("SERVERADD"))
		return MakeIconSpec(enisServerAdd, 47, 170, 121);
	if (name == _T("SERVERSUPDATE"))
		return MakeIconSpec(enisServerUpdate, 58, 150, 208);
	if (name == _T("FOLDEROPEN"))
		return MakeIconSpec(enisFolderOpen, 239, 153, 72);
	if (name == _T("SPEED") || name.Left(5) == _T("SPEED"))
		return MakeIconSpec(enisSpeed, 239, 153, 72);

	// Search uses two icon menus: one selects the search network and the other
	// narrows the results by file type. Resolve these resource names before the
	// generic "SEARCH" rule below so each choice gets a meaningful symbol.
	if (name == _T("SEARCHMETHOD_KADSERVER"))
		return MakeIconSpec(enisServerSearch, 69, 167, 196);
	if (name == _T("SEARCHMETHOD_SERVER"))
		return MakeIconSpec(enisServer, 58, 150, 208);
	if (name == _T("SEARCHMETHOD_GLOBAL"))
		return MakeIconSpec(enisWeb, 69, 167, 196);
	if (name == _T("SEARCHMETHOD_KADEMLIA"))
		return MakeIconSpec(enisNetwork, 116, 104, 215);
	if (name == _T("SEARCHMETHOD_CONTENTDB"))
		return MakeIconSpec(enisDisk, 111, 122, 144);
	if (name == _T("SEARCHFILETYPE_ANY") || name == _T("SEARCHFILETYPE_DOCUMENT") || name == _T("AABCOLLECTIONFILETYPE"))
		return MakeIconSpec(enisFile, 85, 132, 214);
	if (name == _T("SEARCHFILETYPE_ARCHIVE"))
		return MakeIconSpec(enisFileArchive, 239, 153, 72);
	if (name == _T("SEARCHFILETYPE_AUDIO"))
		return MakeIconSpec(enisFileAudio, 145, 104, 218);
	if (name == _T("SEARCHFILETYPE_CDIMAGE"))
		return MakeIconSpec(enisFileDisc, 85, 132, 214);
	if (name == _T("SEARCHFILETYPE_PICTURE"))
		return MakeIconSpec(enisFilePicture, 72, 176, 151);
	if (name == _T("SEARCHFILETYPE_PROGRAM"))
		return MakeIconSpec(enisFileProgram, 58, 150, 208);
	if (name == _T("SEARCHFILETYPE_VIDEO"))
		return MakeIconSpec(enisFileVideo, 224, 94, 105);
	// Kad routing contacts deliberately share one fixed three-node shape. Only
	// its colour changes with the contact type, so status changes do not make
	// the symbol jump or appear to become a different icon.
	if (name == _T("CONTACT0"))
		return MakeIconSpec(enisNetwork, 47, 170, 121);
	if (name == _T("CONTACT1"))
		return MakeIconSpec(enisNetwork, 69, 167, 196);
	if (name == _T("CONTACT2"))
		return MakeIconSpec(enisNetwork, 226, 180, 65);
	if (name == _T("CONTACT3"))
		return MakeIconSpec(enisNetwork, 239, 135, 62);
	if (name == _T("CONTACT4"))
		return MakeIconSpec(enisNetwork, 224, 94, 105);
	if (name == _T("KADSTATUSDISCONNECTED"))
		return MakeIconSpec(enisKadDisconnected, 224, 94, 105);
	if (name == _T("KADSTATUSCONNECTING"))
		return MakeIconSpec(enisKadConnecting, 239, 153, 72);
	if (name == _T("KADSTATUSFIREWALLED"))
		return MakeIconSpec(enisKadFirewalled, 224, 140, 65);
	if (name == _T("KADSTATUSCONNECTED"))
		return MakeIconSpec(enisKadConnected, 47, 170, 121);
	// The main window and notification-area quick menu share one scalable eMule
	// identity icon. Keep it separate from Kad status glyphs, whose colour
	// communicates state.
	if (name == _T("NEXT_MULE_HEAD"))
		return MakeIconSpec(enisMuleHead, 196, 112, 72);
	// Keep the notification-area identity recognizable as eMule Next while
	// preserving the connection feedback previously carried by three legacy
	// tray pictures.
	if (name == _T("NEXT_MULE_TRAY_CONNECTED"))
		return MakeIconSpec(enisMuleHead, 47, 170, 121);
	if (name == _T("NEXT_MULE_TRAY_LOWID"))
		return MakeIconSpec(enisMuleHead, 239, 153, 72);
	if (name == _T("NEXT_MULE_TRAY_DISCONNECTED"))
		return MakeIconSpec(enisMuleHead, 111, 122, 144);
	// eD2K is server based, unlike the distributed Kad network.  Keep the
	// connection-state colours while using a server glyph for this status pane.
	if (name == _T("ED2KSTATUSDISCONNECTED"))
		return MakeIconSpec(enisServer, 224, 94, 105);
	if (name == _T("ED2KSTATUSLOWID"))
		return MakeIconSpec(enisServer, 239, 153, 72);
	if (name == _T("ED2KSTATUSHIGHID"))
		return MakeIconSpec(enisServer, 47, 170, 121);
	// The status-bar transfer indicator has four explicit states. Do not let the
	// generic UP/DOWN rules below collapse them into a single arrow.
	if (name == _T("UP0DOWN0"))
		return MakeIconSpec(enisTransfer, 111, 122, 144);
	if (name == _T("UP0DOWN1"))
		return MakeIconSpec(enisDownload, 58, 150, 208);
	if (name == _T("UP1DOWN0"))
		return MakeIconSpec(enisUpload, 72, 176, 151);
	if (name == _T("UP1DOWN1"))
		return MakeIconSpec(enisTransfer, 54, 153, 207);
	if (name.Left(9) == _T("CONNECTED") || name == _T("CONNECTION") || name.Left(4) == _T("TRAY")) {
		if (name.Find(_T("HIGH")) >= 0 || name == _T("TRAYCONNECTED"))
			return MakeIconSpec(enisNetwork, 47, 170, 121);
		if (name.Find(_T("LOW")) >= 0 || name == _T("TRAYLOWID"))
			return MakeIconSpec(enisNetwork, 239, 153, 72);
		return MakeIconSpec(enisNetwork, 224, 94, 105);
	}
	if (name.Left(6) == _T("EXPAND"))
		return MakeIconSpec(enisAdd, 47, 170, 121);
	if (name.Left(8) == _T("COLLAPSE") || name.Left(5) == _T("CLEAR"))
		return MakeIconSpec(enisRemove, 224, 94, 105);
	if (name.Left(5) == _T("CLOSET"))
		return MakeIconSpec(enisRemove, 224, 94, 105);
	if (name == _T("BOLD") || name == _T("ITALIC") || name == _T("UNDERLINE") || name == _T("COLOUR") || name == _T("RESETFORMAT"))
		return MakeIconSpec(enisEdit, 145, 104, 218);

	if (name.Find(_T("RATING")) >= 0 || name == _T("RATING"))
		return MakeIconSpec(enisStar, 242, 181, 60);
	if (name.Find(_T("SEARCH")) >= 0 || name == _T("SEARCHEDIT"))
		return MakeIconSpec(enisSearch, 69, 167, 196);
	if (name.Find(_T("KAD")) >= 0 || name.Find(_T("CONTACT")) >= 0)
		return MakeIconSpec(enisNetwork, 116, 104, 215);
	if (name.Find(_T("SERVER")) >= 0)
		return MakeIconSpec(enisServer, 58, 150, 208);
	if (name.Find(_T("STAT")) >= 0)
		return MakeIconSpec(enisStatistics, 72, 176, 151);
	if (name.Find(_T("TRANSFER")) >= 0 || name == _T("SPLITWINDOW"))
		return MakeIconSpec(enisTransfer, 54, 153, 207);
	if (name.Find(_T("UPLOAD")) >= 0 || name.Find(_T("UP1")) >= 0 || name.Find(_T("UP0")) >= 0)
		return MakeIconSpec(enisUpload, 72, 176, 151);
	if (name.Find(_T("DOWNLOAD")) >= 0 || name == _T("DIRECTDOWNLOAD"))
		return MakeIconSpec(enisDownload, 58, 150, 208);
	if (name.Find(_T("SHARED")) >= 0 || name == _T("ALLFILES") || name == _T("INCOMING") || name == _T("INCOMPLETE"))
		return MakeIconSpec(enisShare, 239, 153, 72);
	if (name.Find(_T("FOLDER")) >= 0 || name == _T("FOLDERS"))
		return MakeIconSpec(enisFolder, 239, 153, 72);
	if (name.Find(_T("FILETYPE")) >= 0 || name.Find(_T("FILE")) >= 0 || name.Find(_T("COLLECTION")) >= 0 || name == _T("EMPTY"))
		return MakeIconSpec(enisFile, 85, 132, 214);
	if (name.Find(_T("COLL")) >= 0 || name == _T("IMPORT"))
		return MakeIconSpec(enisFile, 85, 132, 214);
	if (name.Find(_T("MESSAGE")) >= 0 || name == _T("CHAT") || name == _T("IRC") || name == _T("IRCCLIPBOARD"))
		return MakeIconSpec(enisChat, 145, 104, 218);
	if (name.Find(_T("CLIENT")) >= 0 || name.Find(_T("FRIEND")) >= 0 || name.Left(3) == _T("SRC") || name == _T("USER"))
		return MakeIconSpec(enisUser, 85, 132, 214);
	if (name.Find(_T("SECURE")) >= 0 || name.Find(_T("SECURITY")) >= 0 || name.Find(_T("OBFU")) >= 0 || name.Find(_T("NOACCESS")) >= 0 || name == _T("IPFILTER"))
		return MakeIconSpec(enisSecurity, 74, 108, 199);
	if (name == _T("PAUSE"))
		return MakeIconSpec(enisPause, 239, 153, 72);
	if (name == _T("STOP") || name == _T("EXIT"))
		return MakeIconSpec(enisStop, 224, 94, 105);
	if (name == _T("RESUME") || name == _T("START"))
		return MakeIconSpec(enisPlay, 47, 170, 121);
	if (name == _T("PREVIEW") || name == _T("ARCHIVE_PREVIEW"))
		return MakeIconSpec(enisPreview, 69, 167, 196);
	if (name.Find(_T("LINK")) >= 0)
		return MakeIconSpec(enisLink, 85, 132, 214);
	if (name == _T("MAIL") || name == _T("EMAIL"))
		return MakeIconSpec(enisMail, 145, 104, 218);
	if (name == _T("BROWSE8") || name.Find(_T("PREVIEW")) >= 0)
		return MakeIconSpec(enisPreview, 69, 167, 196);
	if (name.Find(_T("LIST")) >= 0 || name == _T("LOG"))
		return MakeIconSpec(enisList, 111, 122, 144);
	if (name.Find(_T("RENAME")) >= 0 || name == _T("EDIT"))
		return MakeIconSpec(enisEdit, 85, 132, 214);
	if (name.Find(_T("PRIORITY")) >= 0)
		return MakeIconSpec(enisPriority, 239, 153, 72);
	if (name == _T("CATEGORY"))
		return MakeIconSpec(enisCategory, 145, 104, 218);
	if (name == _T("SPAM") || name == _T("WARNING"))
		return MakeIconSpec(enisWarning, 224, 140, 65);

	return MakeIconSpec(enisUnknown, 0, 0, 0);
}

static void DrawNextIcon(Gdiplus::Graphics& graphics, int iconSize, const SNextIconSpec& spec)
{
	const float scale = static_cast<float>(iconSize) / 32.0f;
	const auto P = [scale](float value) { return value * scale; };
	const auto Rect = [&P](float left, float top, float right, float bottom) {
		return Gdiplus::RectF(P(left), P(top), P(right) - P(left), P(bottom) - P(top));
	};
	const Gdiplus::Color softFill(48, spec.color.GetR(), spec.color.GetG(), spec.color.GetB());
	Gdiplus::Pen pen(spec.color, 2.35f * scale);
	pen.SetLineJoin(Gdiplus::LineJoinRound);
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);
	Gdiplus::SolidBrush brush(spec.color);
	Gdiplus::SolidBrush fillBrush(softFill);

	switch (spec.symbol) {
	case enisConnect:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(16), P(10), P(16), P(22));
		graphics.DrawLine(&pen, P(11), P(15), P(16), P(10));
		graphics.DrawLine(&pen, P(21), P(15), P(16), P(10));
		break;
	case enisDisconnect:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(11), P(11), P(21), P(21));
		graphics.DrawLine(&pen, P(21), P(11), P(11), P(21));
		break;
	case enisKadDisconnected:
	case enisKadConnecting:
	case enisKadFirewalled:
	case enisKadConnected:
		// Kad is a distributed network: three connected nodes make that role
		// immediately clear and remain legible in the 16px status bar.
		graphics.DrawLine(&pen, P(10), P(10), P(22), P(10));
		graphics.DrawLine(&pen, P(10), P(10), P(16), P(22));
		graphics.DrawLine(&pen, P(22), P(10), P(16), P(22));
		graphics.FillEllipse(&brush, Rect(6, 6, 14, 14));
		graphics.FillEllipse(&brush, Rect(18, 6, 26, 14));
		graphics.FillEllipse(&brush, Rect(12, 18, 20, 26));
		if (spec.symbol == enisKadFirewalled) {
			graphics.DrawLine(&pen, P(7), P(25), P(25), P(7));
		}
		else if (spec.symbol == enisKadConnecting) {
			graphics.DrawArc(&pen, Rect(7, 7, 25, 25), 35, 230);
			graphics.DrawLine(&pen, P(24), P(9), P(24), P(14));
		}
		break;
	case enisMuleHead:
		{
			// A compact front-facing mule: tall ears, long head and light muzzle.
			// The simple silhouette stays readable in a 16-pixel popup-menu icon.
			Gdiplus::PointF leftEar[] = { { P(10), P(11) }, { P(5), P(3) }, { P(12), P(7) } };
			Gdiplus::PointF rightEar[] = { { P(22), P(11) }, { P(27), P(3) }, { P(20), P(7) } };
			graphics.FillPolygon(&fillBrush, leftEar, _countof(leftEar));
			graphics.DrawPolygon(&pen, leftEar, _countof(leftEar));
			graphics.FillPolygon(&fillBrush, rightEar, _countof(rightEar));
			graphics.DrawPolygon(&pen, rightEar, _countof(rightEar));
			graphics.FillEllipse(&fillBrush, Rect(8, 6, 24, 28));
			graphics.DrawEllipse(&pen, Rect(8, 6, 24, 28));
			graphics.FillEllipse(&brush, Rect(11, 12, 14, 15));
			graphics.FillEllipse(&brush, Rect(18, 12, 21, 15));
			graphics.DrawEllipse(&pen, Rect(11, 18, 21, 26));
			graphics.FillEllipse(&brush, Rect(13, 21, 15, 23));
			graphics.FillEllipse(&brush, Rect(17, 21, 19, 23));
		}
		break;
	case enisNetwork:
		graphics.DrawLine(&pen, P(10), P(11), P(22), P(11));
		graphics.DrawLine(&pen, P(10), P(11), P(16), P(23));
		graphics.DrawLine(&pen, P(22), P(11), P(16), P(23));
		graphics.FillEllipse(&brush, Rect(6, 7, 14, 15));
		graphics.FillEllipse(&brush, Rect(18, 7, 26, 15));
		graphics.FillEllipse(&brush, Rect(12, 19, 20, 27));
		break;
	case enisServer:
	case enisServerSearch:
	case enisServerAdd:
	case enisServerUpdate:
		graphics.DrawRectangle(&pen, Rect(5, 6, 27, 13));
		graphics.DrawRectangle(&pen, Rect(5, 18, 27, 25));
		graphics.FillEllipse(&brush, Rect(8, 8, 11, 11));
		graphics.FillEllipse(&brush, Rect(8, 20, 11, 23));
		graphics.DrawLine(&pen, P(15), P(9), P(23), P(9));
		graphics.DrawLine(&pen, P(15), P(21), P(23), P(21));
		if (spec.symbol == enisServerSearch) {
			graphics.DrawEllipse(&pen, Rect(19, 19, 27, 27));
			graphics.DrawLine(&pen, P(25), P(25), P(29), P(29));
		}
		else if (spec.symbol == enisServerAdd) {
			graphics.FillEllipse(&brush, Rect(20, 20, 30, 30));
			graphics.DrawLine(&pen, P(25), P(22), P(25), P(28));
			graphics.DrawLine(&pen, P(22), P(25), P(28), P(25));
		}
		else if (spec.symbol == enisServerUpdate) {
			graphics.DrawArc(&pen, Rect(19, 19, 29, 29), 35, 260);
			graphics.DrawLine(&pen, P(29), P(21), P(29), P(26));
			graphics.DrawLine(&pen, P(29), P(21), P(24), P(21));
		}
		break;
	case enisSearch:
		graphics.DrawEllipse(&pen, Rect(5, 5, 20, 20));
		graphics.DrawLine(&pen, P(17), P(17), P(27), P(27));
		break;
	case enisTransfer:
		graphics.DrawLine(&pen, P(11), P(7), P(11), P(25));
		graphics.DrawLine(&pen, P(8), P(10), P(11), P(7));
		graphics.DrawLine(&pen, P(14), P(10), P(11), P(7));
		graphics.DrawLine(&pen, P(21), P(25), P(21), P(7));
		graphics.DrawLine(&pen, P(18), P(22), P(21), P(25));
		graphics.DrawLine(&pen, P(24), P(22), P(21), P(25));
		break;
	case enisUpload:
	case enisPriority:
		graphics.DrawLine(&pen, P(16), P(26), P(16), P(7));
		graphics.DrawLine(&pen, P(9), P(14), P(16), P(7));
		graphics.DrawLine(&pen, P(23), P(14), P(16), P(7));
		break;
	case enisDownload:
		graphics.DrawLine(&pen, P(16), P(6), P(16), P(25));
		graphics.DrawLine(&pen, P(9), P(18), P(16), P(25));
		graphics.DrawLine(&pen, P(23), P(18), P(16), P(25));
		break;
	case enisShare:
	case enisFolder:
	case enisFolderOpen:
		graphics.DrawLine(&pen, P(4), P(11), P(12), P(11));
		graphics.DrawLine(&pen, P(12), P(11), P(15), P(8));
		graphics.DrawLine(&pen, P(15), P(8), P(27), P(8));
		graphics.DrawLine(&pen, P(27), P(8), P(27), P(25));
		graphics.DrawLine(&pen, P(27), P(25), P(4), P(25));
		graphics.DrawLine(&pen, P(4), P(25), P(4), P(11));
		graphics.FillRectangle(&fillBrush, Rect(6, 14, 25, 23));
		if (spec.symbol == enisShare)
			graphics.DrawLine(&pen, P(16), P(15), P(16), P(23));
		else if (spec.symbol == enisFolderOpen)
			graphics.DrawLine(&pen, P(7), P(17), P(24), P(13));
		break;
	case enisChat:
		{
			Gdiplus::PointF points[] = { { P(5), P(7) }, { P(27), P(7) }, { P(27), P(21) }, { P(16), P(21) }, { P(10), P(26) }, { P(11), P(21) }, { P(5), P(21) } };
			graphics.DrawPolygon(&pen, points, _countof(points));
			graphics.DrawLine(&pen, P(10), P(13), P(22), P(13));
			graphics.DrawLine(&pen, P(10), P(17), P(18), P(17));
		}
		break;
	case enisStatistics:
		graphics.DrawLine(&pen, P(5), P(27), P(27), P(27));
		graphics.FillRectangle(&brush, Rect(7, 18, 11, 27));
		graphics.FillRectangle(&brush, Rect(14, 12, 18, 27));
		graphics.FillRectangle(&brush, Rect(21, 6, 25, 27));
		break;
	case enisSettings:
		for (int i = 0; i < 3; ++i)
			graphics.DrawLine(&pen, P(5), P(8.0f + static_cast<float>(i) * 8.0f), P(27), P(8.0f + static_cast<float>(i) * 8.0f));
		graphics.FillEllipse(&brush, Rect(11, 5, 17, 11));
		graphics.FillEllipse(&brush, Rect(19, 13, 25, 19));
		graphics.FillEllipse(&brush, Rect(8, 21, 14, 27));
		break;
	case enisTools:
		graphics.DrawLine(&pen, P(9), P(23), P(23), P(9));
		graphics.DrawEllipse(&pen, Rect(18, 4, 28, 14));
		graphics.FillEllipse(&brush, Rect(6, 20, 12, 26));
		break;
	case enisHelp:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawArc(&pen, Rect(10, 9, 22, 21), 180, 180);
		graphics.DrawLine(&pen, P(21), P(14), P(16), P(18));
		graphics.DrawLine(&pen, P(16), P(18), P(16), P(21));
		graphics.FillEllipse(&brush, Rect(14, 23, 18, 27));
		break;
	case enisUser:
	case enisUserAdd:
	case enisUserRemove:
	case enisUserMessage:
	case enisUserFiles:
		graphics.FillEllipse(&brush, Rect(11, 5, 21, 15));
		graphics.DrawArc(&pen, Rect(6, 13, 26, 29), 200, 140);
		if (spec.symbol == enisUserAdd) {
			graphics.FillEllipse(&brush, Rect(20, 20, 30, 30));
			graphics.DrawLine(&pen, P(25), P(22), P(25), P(28));
			graphics.DrawLine(&pen, P(22), P(25), P(28), P(25));
		}
		else if (spec.symbol == enisUserRemove) {
			graphics.FillEllipse(&brush, Rect(20, 20, 30, 30));
			graphics.DrawLine(&pen, P(22), P(25), P(28), P(25));
		}
		else if (spec.symbol == enisUserMessage) {
			graphics.DrawRectangle(&pen, Rect(20, 20, 30, 27));
			graphics.DrawLine(&pen, P(23), P(27), P(21), P(30));
		}
		else if (spec.symbol == enisUserFiles) {
			graphics.DrawRectangle(&pen, Rect(20, 20, 30, 28));
			graphics.DrawLine(&pen, P(20), P(22), P(24), P(22));
		}
		break;
	case enisUsers:
		// A compact group makes the transfer queue recognizable in both the
		// download and upload selectors, even at the 16-pixel toolbar size.
		graphics.FillEllipse(&brush, Rect(12, 4, 20, 12));
		graphics.DrawArc(&pen, Rect(8, 10, 24, 27), 200, 140);
		graphics.FillEllipse(&brush, Rect(4, 9, 11, 16));
		graphics.DrawArc(&pen, Rect(1, 15, 14, 29), 205, 130);
		graphics.FillEllipse(&brush, Rect(21, 9, 28, 16));
		graphics.DrawArc(&pen, Rect(18, 15, 31, 29), 205, 130);
		break;
	case enisFile:
		graphics.DrawRectangle(&pen, Rect(7, 4, 24, 28));
		graphics.DrawLine(&pen, P(11), P(11), P(20), P(11));
		graphics.DrawLine(&pen, P(11), P(16), P(20), P(16));
		graphics.DrawLine(&pen, P(11), P(21), P(17), P(21));
		break;
	case enisFileAudio:
		graphics.DrawRectangle(&pen, Rect(7, 4, 24, 28));
		graphics.DrawLine(&pen, P(18), P(10), P(18), P(22));
		graphics.DrawLine(&pen, P(18), P(10), P(23), P(12));
		graphics.FillEllipse(&brush, Rect(12, 19, 18, 25));
		break;
	case enisFileVideo:
		graphics.DrawRectangle(&pen, Rect(5, 7, 27, 25));
		{
			Gdiplus::PointF points[] = { { P(13), P(11) }, { P(22), P(16) }, { P(13), P(21) } };
			graphics.FillPolygon(&brush, points, _countof(points));
		}
		break;
	case enisFilePicture:
		graphics.DrawRectangle(&pen, Rect(5, 6, 27, 25));
		graphics.FillEllipse(&brush, Rect(8, 9, 13, 14));
		graphics.DrawLine(&pen, P(7), P(23), P(14), P(16));
		graphics.DrawLine(&pen, P(14), P(16), P(19), P(21));
		graphics.DrawLine(&pen, P(19), P(21), P(23), P(17));
		graphics.DrawLine(&pen, P(23), P(17), P(26), P(23));
		break;
	case enisFileArchive:
		graphics.DrawRectangle(&pen, Rect(6, 8, 26, 26));
		graphics.DrawLine(&pen, P(6), P(13), P(26), P(13));
		graphics.DrawLine(&pen, P(16), P(13), P(16), P(23));
		graphics.FillRectangle(&brush, Rect(14, 16, 18, 18));
		graphics.FillRectangle(&brush, Rect(14, 21, 18, 23));
		break;
	case enisFileDisc:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawEllipse(&pen, Rect(12, 12, 20, 20));
		graphics.FillEllipse(&brush, Rect(15, 15, 17, 17));
		break;
	case enisFileProgram:
		graphics.DrawRectangle(&pen, Rect(5, 6, 27, 26));
		graphics.DrawLine(&pen, P(5), P(11), P(27), P(11));
		graphics.FillEllipse(&brush, Rect(8, 8, 10, 10));
		graphics.DrawLine(&pen, P(11), P(16), P(8), P(19));
		graphics.DrawLine(&pen, P(8), P(19), P(11), P(22));
		graphics.DrawLine(&pen, P(21), P(16), P(24), P(19));
		graphics.DrawLine(&pen, P(24), P(19), P(21), P(22));
		break;
	case enisPlay:
		{
			Gdiplus::PointF points[] = { { P(10), P(6) }, { P(26), P(16) }, { P(10), P(26) } };
			graphics.FillPolygon(&brush, points, _countof(points));
		}
		break;
	case enisPause:
		graphics.FillRectangle(&brush, Rect(8, 6, 13, 26));
		graphics.FillRectangle(&brush, Rect(19, 6, 24, 26));
		break;
	case enisStop:
		graphics.FillRectangle(&brush, Rect(7, 7, 25, 25));
		break;
	case enisAdd:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(16), P(10), P(16), P(22));
		graphics.DrawLine(&pen, P(10), P(16), P(22), P(16));
		break;
	case enisRemove:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(10), P(16), P(22), P(16));
		break;
	case enisCopy:
		graphics.DrawRectangle(&pen, Rect(7, 5, 22, 21));
		graphics.DrawRectangle(&pen, Rect(11, 10, 26, 27));
		break;
	case enisDelete:
		graphics.DrawLine(&pen, P(9), P(9), P(23), P(9));
		graphics.DrawLine(&pen, P(13), P(6), P(19), P(6));
		graphics.DrawRectangle(&pen, Rect(10, 10, 22, 27));
		graphics.DrawLine(&pen, P(14), P(14), P(14), P(23));
		graphics.DrawLine(&pen, P(18), P(14), P(18), P(23));
		break;
	case enisStar:
		{
			Gdiplus::PointF points[] = { { P(16), P(4) }, { P(19), P(12) }, { P(28), P(12) }, { P(21), P(17) }, { P(24), P(27) }, { P(16), P(21) }, { P(8), P(27) }, { P(11), P(17) }, { P(4), P(12) }, { P(13), P(12) } };
			graphics.FillPolygon(&brush, points, _countof(points));
		}
		break;
	case enisSecurity:
		{
			Gdiplus::PointF points[] = { { P(16), P(4) }, { P(25), P(8) }, { P(23), P(19) }, { P(16), P(28) }, { P(9), P(19) }, { P(7), P(8) } };
			graphics.DrawPolygon(&pen, points, _countof(points));
			graphics.FillRectangle(&brush, Rect(12, 16, 20, 23));
			graphics.DrawArc(&pen, Rect(13, 11, 6, 9), 180, 180);
		}
		break;
	case enisClock:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(16), P(16), P(16), P(9));
		graphics.DrawLine(&pen, P(16), P(16), P(22), P(19));
		graphics.FillEllipse(&brush, Rect(14, 14, 18, 18));
		break;
	case enisWeb:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.DrawLine(&pen, P(5), P(16), P(27), P(16));
		graphics.DrawArc(&pen, Rect(9, 5, 14, 22), 270, 180);
		graphics.DrawArc(&pen, Rect(9, 5, 14, 22), 90, 180);
		break;
	case enisBell:
		graphics.DrawArc(&pen, Rect(9, 6, 14, 17), 180, 180);
		graphics.DrawLine(&pen, P(9), P(15), P(7), P(22));
		graphics.DrawLine(&pen, P(23), P(15), P(25), P(22));
		graphics.DrawLine(&pen, P(7), P(22), P(25), P(22));
		graphics.FillEllipse(&brush, Rect(14, 24, 18, 28));
		break;
	case enisInfo:
		graphics.DrawEllipse(&pen, Rect(5, 5, 27, 27));
		graphics.FillEllipse(&brush, Rect(14, 9, 18, 13));
		graphics.DrawLine(&pen, P(16), P(16), P(16), P(23));
		break;
	case enisLink:
		graphics.DrawArc(&pen, Rect(5, 11, 19, 23), 135, 180);
		graphics.DrawArc(&pen, Rect(13, 9, 27, 21), 315, 180);
		graphics.DrawLine(&pen, P(11), P(16), P(21), P(16));
		break;
	case enisMail:
		graphics.DrawRectangle(&pen, Rect(4, 7, 28, 25));
		graphics.DrawLine(&pen, P(5), P(8), P(16), P(17));
		graphics.DrawLine(&pen, P(27), P(8), P(16), P(17));
		break;
	case enisDisk:
		graphics.DrawRectangle(&pen, Rect(5, 5, 27, 27));
		graphics.FillRectangle(&fillBrush, Rect(8, 8, 24, 16));
		graphics.DrawRectangle(&pen, Rect(10, 19, 22, 26));
		break;
	case enisPreview:
		graphics.DrawEllipse(&pen, Rect(5, 10, 27, 22));
		graphics.FillEllipse(&brush, Rect(13, 12, 19, 20));
		break;
	case enisList:
		for (int i = 0; i < 3; ++i) {
			const float y = 8.0f + static_cast<float>(i) * 8.0f;
			graphics.FillEllipse(&brush, Rect(5, y - 2, 9, y + 2));
			graphics.DrawLine(&pen, P(12), P(y), P(27), P(y));
		}
		break;
	case enisFilter:
		{
			Gdiplus::PointF points[] = { { P(5), P(7) }, { P(27), P(7) }, { P(19), P(16) }, { P(19), P(25) }, { P(13), P(28) }, { P(13), P(16) } };
			graphics.DrawPolygon(&pen, points, _countof(points));
			graphics.FillPolygon(&fillBrush, points, _countof(points));
		}
		break;
	case enisSpeed:
		graphics.DrawArc(&pen, Rect(5, 7, 27, 29), 205, 130);
		graphics.DrawLine(&pen, P(16), P(20), P(22), P(13));
		graphics.FillEllipse(&brush, Rect(14, 18, 18, 22));
		graphics.DrawLine(&pen, P(8), P(18), P(10), P(17));
		graphics.DrawLine(&pen, P(22), P(17), P(24), P(18));
		break;
	case enisEdit:
		graphics.DrawLine(&pen, P(8), P(24), P(23), P(9));
		graphics.DrawLine(&pen, P(20), P(7), P(25), P(12));
		graphics.DrawLine(&pen, P(7), P(25), P(13), P(23));
		break;
	case enisCategory:
		{
			Gdiplus::PointF points[] = { { P(5), P(10) }, { P(16), P(5) }, { P(27), P(10) }, { P(16), P(16) } };
			graphics.FillPolygon(&brush, points, _countof(points));
			graphics.DrawLine(&pen, P(5), P(16), P(16), P(22));
			graphics.DrawLine(&pen, P(16), P(22), P(27), P(16));
		}
		break;
	case enisWarning:
		{
			Gdiplus::PointF points[] = { { P(16), P(5) }, { P(28), P(27) }, { P(4), P(27) } };
			graphics.DrawPolygon(&pen, points, _countof(points));
			graphics.DrawLine(&pen, P(16), P(12), P(16), P(20));
			graphics.FillEllipse(&brush, Rect(14, 22, 18, 26));
		}
		break;
	case enisBack:
		graphics.DrawLine(&pen, P(26), P(16), P(7), P(16));
		graphics.DrawLine(&pen, P(15), P(8), P(7), P(16));
		graphics.DrawLine(&pen, P(15), P(24), P(7), P(16));
		break;
	case enisForward:
		graphics.DrawLine(&pen, P(6), P(16), P(25), P(16));
		graphics.DrawLine(&pen, P(17), P(8), P(25), P(16));
		graphics.DrawLine(&pen, P(17), P(24), P(25), P(16));
		break;
	case enisSortUp:
		graphics.DrawLine(&pen, P(7), P(21), P(16), P(12));
		graphics.DrawLine(&pen, P(16), P(12), P(25), P(21));
		break;
	case enisSortDown:
		graphics.DrawLine(&pen, P(7), P(11), P(16), P(20));
		graphics.DrawLine(&pen, P(16), P(20), P(25), P(11));
		break;
	case enisSortDoubleUp:
		graphics.DrawLine(&pen, P(8), P(16), P(16), P(8));
		graphics.DrawLine(&pen, P(16), P(8), P(24), P(16));
		graphics.DrawLine(&pen, P(8), P(25), P(16), P(17));
		graphics.DrawLine(&pen, P(16), P(17), P(24), P(25));
		break;
	case enisSortDoubleDown:
		graphics.DrawLine(&pen, P(8), P(7), P(16), P(15));
		graphics.DrawLine(&pen, P(16), P(15), P(24), P(7));
		graphics.DrawLine(&pen, P(8), P(16), P(16), P(24));
		graphics.DrawLine(&pen, P(16), P(24), P(24), P(16));
		break;
	default:
		break;
	}
}
}

bool CreateNextModernIcon(LPCTSTR pszResourceName, int cx, int cy, HICON* phIcon)
{
	if (phIcon == NULL)
		return false;
	*phIcon = NULL;

	const SNextIconSpec spec = ResolveNextIcon(pszResourceName);
	if (spec.symbol == enisUnknown)
		return false;

	static CNextIconGdiplusRuntime gdiplusRuntime;
	if (!gdiplusRuntime.IsReady())
		return false;

	const int width = cx > 0 ? cx : 16;
	const int height = cy > 0 ? cy : 16;
	const int canvasSize = width > height ? width : height;
	const int supersampling = canvasSize <= 32 ? 4 : 2;
	Gdiplus::Bitmap source(canvasSize * supersampling, canvasSize * supersampling, PixelFormat32bppPARGB);
	Gdiplus::Bitmap output(width, height, PixelFormat32bppPARGB);
	if (source.GetLastStatus() != Gdiplus::Ok || output.GetLastStatus() != Gdiplus::Ok)
		return false;

	Gdiplus::Graphics sourceGraphics(&source);
	sourceGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	sourceGraphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	sourceGraphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	sourceGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
	DrawNextIcon(sourceGraphics, canvasSize * supersampling, spec);

	Gdiplus::Graphics outputGraphics(&output);
	outputGraphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	outputGraphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	outputGraphics.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	outputGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));
	outputGraphics.DrawImage(&source, Gdiplus::Rect(0, 0, width, height), 0, 0,
		source.GetWidth(), source.GetHeight(), Gdiplus::UnitPixel);

	return output.GetHICON(phIcon) == Gdiplus::Ok && *phIcon != NULL;
}
