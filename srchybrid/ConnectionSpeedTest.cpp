// eMule Next - GPL-2.0-or-later
#include "stdafx.h"
#include "ConnectionSpeedTest.h"
#include "opcodes.h"
#include <wininet.h>
#include <vector>

namespace
{
class InternetHandle
{
public:
	explicit InternetHandle(HINTERNET handle = NULL) : m_handle(handle) {}
	~InternetHandle()
	{
		if (m_handle != NULL)
			InternetCloseHandle(m_handle);
	}
	InternetHandle(const InternetHandle&) = delete;
	InternetHandle& operator=(const InternetHandle&) = delete;
	operator HINTERNET() const { return m_handle; }
	bool IsValid() const { return m_handle != NULL; }

private:
	HINTERNET m_handle;
};

struct Sample
{
	uint32 rateKBps = 0;
	DWORD durationMs = 0;
};

DWORD ElapsedMilliseconds(const LARGE_INTEGER& start, const LARGE_INTEGER& finish, const LARGE_INTEGER& frequency)
{
	if (finish.QuadPart <= start.QuadPart || frequency.QuadPart <= 0)
		return 0;
	const double milliseconds = static_cast<double>(finish.QuadPart - start.QuadPart)
		* 1000.0 / static_cast<double>(frequency.QuadPart);
	return static_cast<DWORD>(milliseconds > MAXDWORD ? MAXDWORD : milliseconds);
}

uint32 RateInKBps(uint64 bytes, DWORD durationMs)
{
	if (bytes == 0 || durationMs == 0)
		return 0;
	const uint64 rate = (bytes * 1000u + static_cast<uint64>(durationMs) * 512u)
		/ (static_cast<uint64>(durationMs) * 1024u);
	const uint64 maximum = static_cast<uint64>(UNLIMITED) - 1u;
	return static_cast<uint32>(rate < maximum ? rate : maximum);
}

bool HasSuccessfulStatus(HINTERNET request)
{
	DWORD status = 0;
	DWORD size = sizeof(status);
	return HttpQueryInfo(request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&status, &size, NULL) != FALSE && status >= 200 && status < 300;
}

void ConfigureTimeouts(HINTERNET session)
{
	DWORD timeout = 15000;
	InternetSetOption(session, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
	InternetSetOption(session, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
	InternetSetOption(session, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
}

bool DownloadSample(HINTERNET session, DWORD requestedBytes, const LARGE_INTEGER& frequency,
	Sample& sample, DWORD& error)
{
	CString url;
	url.Format(_T("https://speed.cloudflare.com/__down?bytes=%lu"), requestedBytes);
	InternetHandle request(InternetOpenUrl(session, url,
		_T("Cache-Control: no-cache\r\n"), static_cast<DWORD>(-1L),
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0));
	if (!request.IsValid()) {
		error = GetLastError();
		return false;
	}
	if (!HasSuccessfulStatus(request)) {
		error = ERROR_HTTP_INVALID_SERVER_RESPONSE;
		return false;
	}

	BYTE buffer[64 * 1024];
	uint64 received = 0;
	LARGE_INTEGER start = {}, finish = {};
	QueryPerformanceCounter(&start);
	for (;;) {
		DWORD count = 0;
		if (!InternetReadFile(request, buffer, sizeof(buffer), &count)) {
			error = GetLastError();
			return false;
		}
		if (count == 0)
			break;
		received += count;
		if (received > requestedBytes) {
			error = ERROR_INVALID_DATA;
			return false;
		}
	}
	QueryPerformanceCounter(&finish);
	if (received != requestedBytes) {
		error = ERROR_HANDLE_EOF;
		return false;
	}
	sample.durationMs = ElapsedMilliseconds(start, finish, frequency);
	sample.rateKBps = RateInKBps(received, sample.durationMs);
	return sample.rateKBps != 0;
}

bool UploadSample(HINTERNET connection, DWORD requestedBytes, const LARGE_INTEGER& frequency,
	Sample& sample, DWORD& error)
{
	static LPCTSTR acceptTypes[] = {_T("*/*"), NULL};
	InternetHandle request(HttpOpenRequest(connection, _T("POST"), _T("/__up"), NULL, NULL,
		acceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE, 0));
	if (!request.IsValid()) {
		error = GetLastError();
		return false;
	}

	std::vector<BYTE> payload(requestedBytes);
	uint32 state = 0x454D554Cu;
	for (DWORD i = 0; i < requestedBytes; ++i) {
		state = state * 1664525u + 1013904223u;
		payload[i] = static_cast<BYTE>(state >> 24);
	}

	LARGE_INTEGER start = {}, finish = {};
	QueryPerformanceCounter(&start);
	const BOOL sent = HttpSendRequest(request,
		_T("Content-Type: application/octet-stream\r\nCache-Control: no-cache\r\n"),
		static_cast<DWORD>(-1L), payload.data(), requestedBytes);
	QueryPerformanceCounter(&finish);
	if (!sent) {
		error = GetLastError();
		return false;
	}
	if (!HasSuccessfulStatus(request)) {
		error = ERROR_HTTP_INVALID_SERVER_RESPONSE;
		return false;
	}
	sample.durationMs = ElapsedMilliseconds(start, finish, frequency);
	sample.rateKBps = RateInKBps(requestedBytes, sample.durationMs);
	return sample.rateKBps != 0;
}
}

ConnectionSpeedTest::Result ConnectionSpeedTest::Run()
{
	Result result;
	InternetHandle session(InternetOpen(_T("eMule Next connection test"),
		INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0));
	if (!session.IsValid()) {
		result.error = GetLastError();
		return result;
	}
	ConfigureTimeouts(session);

	LARGE_INTEGER frequency = {};
	if (!QueryPerformanceFrequency(&frequency)) {
		result.error = GetLastError();
		return result;
	}

	const DWORD downloadSizes[] = {256u * 1024u, 2u * 1024u * 1024u,
		10u * 1024u * 1024u, 50u * 1024u * 1024u};
	for (DWORD size : downloadSizes) {
		Sample sample;
		if (!DownloadSample(session, size, frequency, sample, result.error))
			return result;
		result.downloadKBps = sample.rateKBps;
		if (sample.durationMs >= 750)
			break;
	}

	InternetHandle connection(InternetConnect(session, _T("speed.cloudflare.com"),
		INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0));
	if (!connection.IsValid()) {
		result.error = GetLastError();
		result.downloadKBps = 0;
		return result;
	}
	const DWORD uploadSizes[] = {128u * 1024u, 512u * 1024u, 2u * 1024u * 1024u,
		10u * 1024u * 1024u, 25u * 1024u * 1024u};
	for (DWORD size : uploadSizes) {
		Sample sample;
		if (!UploadSample(connection, size, frequency, sample, result.error)) {
			result.downloadKBps = 0;
			return result;
		}
		result.uploadKBps = sample.rateKBps;
		if (sample.durationMs >= 750)
			break;
	}
	result.error = ERROR_SUCCESS;
	return result;
}
