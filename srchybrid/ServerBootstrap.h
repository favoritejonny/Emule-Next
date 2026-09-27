// eMule Next - GPL-2.0-or-later
#pragma once

namespace ServerBootstrap
{
struct Entry { const wchar_t* address; unsigned short port; const wchar_t* name; };
// The online list is merged after explicit first-run consent. These checked
// entries remain as an offline fallback and never replace existing servers.
static const wchar_t OnlineServerMetUrl[] = L"https://upd.emule-security.org/server.met";
static const Entry Entries[] = {
	{L"176.123.5.89", 4725, L"eMule Sunrise"},
	{L"77.42.68.79", 4232, L"Nordic Server FIN"},
	{L"85.121.5.137", 4232, L"Sharing-Devils No.2"},
	{L"91.208.162.182", 4232, L"MO-Server"},
	{L"45.87.41.16", 6262, L"ed2k-rust test server"},
	{L"91.208.162.87", 4232, L"Sharing-Devils No.4"},
	{L"213.141.198.207", 4232, L"Mazinga Server"},
	{L"85.17.116.222", 6082, L"ed2k-rust main server"}
};

// Startup initialization can run inside the wizard's modal message loop.
// Defer an explicit choice until saved servers have been loaded, if needed.
class Selection
{
public:
	bool Finish(bool completed, bool selected, bool ed2k)
	{
		if (!completed || !selected || !ed2k)
			return false;
		if (m_loaded)
			return true;
		m_pending = true;
		return false;
	}
	bool Loaded()
	{
		m_loaded = true;
		const bool requested = m_pending;
		m_pending = false;
		return requested;
	}
private:
	bool m_loaded = false;
	bool m_pending = false;
};

template<class Exists, class Add>
unsigned MergeMissing(Exists exists, Add add)
{
	unsigned added = 0;
	for (const auto& entry : Entries)
		if (!exists(entry) && add(entry))
			++added;
	return added;
}
}
