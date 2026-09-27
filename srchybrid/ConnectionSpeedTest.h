// eMule Next - GPL-2.0-or-later
#pragma once

namespace ConnectionSpeedTest
{
struct Result
{
	uint32 downloadKBps = 0;
	uint32 uploadKBps = 0;
	DWORD error = ERROR_SUCCESS;

	bool Succeeded() const
	{
		return downloadKBps != 0 && uploadKBps != 0;
	}
};

// Runs only after an explicit click in the first-run wizard. The implementation
// uses the public Cloudflare measurement endpoints but does not submit a result
// record or any eMule data.
Result Run();

inline uint32 RecommendedUploadLimit(uint32 uploadCapacityKBps)
{
	if (uploadCapacityKBps == 0)
		return 0;
	const uint64 recommended = static_cast<uint64>(uploadCapacityKBps) * 4u / 5u;
	return static_cast<uint32>(recommended != 0 ? recommended : 1u);
}
}
