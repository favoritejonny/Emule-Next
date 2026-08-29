//this file is part of eMule Next
#pragma once

#include <windows.h>

// GetTickCount wraps roughly every 49 days. Timers in the client are much
// shorter than half that range, so unsigned subtraction keeps elapsed-time
// comparisons correct on both sides of the wrap.
inline bool HasTickCountElapsed(DWORD currentTick, DWORD startTick, DWORD interval)
{
	return static_cast<DWORD>(currentTick - startTick) >= interval;
}

// Use this for a timestamp which represents a future deadline. A zero value
// is the conventional "no deadline yet" sentinel in the existing code.
inline bool IsTickCountDue(DWORD currentTick, DWORD dueTick)
{
	return dueTick == 0 || static_cast<DWORD>(currentTick - dueTick) < 0x80000000UL;
}
