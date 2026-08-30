// This file is part of eMule Next and is licensed under GPL-2.0-or-later.
#pragma once

#include "opcodes.h"

namespace UploadPolicy
{
	inline uint32 GetTargetClientDataRate(uint32 openSlots, bool minimumRate)
	{
		// Up to three slots target 3 KiB/s. Above that, grow linearly and
		// retain the protocol's long-standing per-client ceiling.
		uint32 result = openSlots <= 3
			? 3u * 1024u
			: min(UPLOAD_CLIENT_MAXDATARATE, openSlots * 1024u);
		return minimumRate ? result * 3u / 4u : result;
	}

	inline uint32 GetSlotLimit(uint32 currentUploadSpeed, uint32 targetRate)
	{
		if (currentUploadSpeed > 49u * 1024u) {
			targetRate += currentUploadSpeed / 43u;
			if (targetRate > UPLOAD_CLIENT_MAXDATARATE)
				targetRate = UPLOAD_CLIENT_MAXDATARATE;
		}

		if (currentUploadSpeed > 25u * 1024u)
			return max(currentUploadSpeed / targetRate, MIN_UP_CLIENTS_ALLOWED + 3u);
		if (currentUploadSpeed > 16u * 1024u)
			return MIN_UP_CLIENTS_ALLOWED + 2u;
		if (currentUploadSpeed > 9u * 1024u)
			return MIN_UP_CLIENTS_ALLOWED + 1u;
		return MIN_UP_CLIENTS_ALLOWED;
	}

	inline bool ShouldOpenUnlimitedSlot(uint32 currentSlots, uint32 currentUploadSpeed, uint32 targetRate)
	{
		const uint32 minimumUnlimitedSlots = MIN_UP_CLIENTS_ALLOWED + 3u;
		return currentSlots < minimumUnlimitedSlots
			|| (targetRate != 0 && currentSlots < currentUploadSpeed / targetRate);
	}
}
