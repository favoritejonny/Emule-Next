#pragma once

// Private diagnostics are compiled only into explicitly requested test builds.
// Public and normal local builds compile every call below to a no-op.
class PrivateDiagnostics
{
public:
#ifdef EMULENEXT_PRIVATE_DIAGNOSTICS
	static void Initialize();
	static void Shutdown();
	static void Sample();
	static void LogEvent(const char* category, const char* action,
		uint64 value1 = 0, uint64 value2 = 0, uint64 value3 = 0);
	static void LogUiCommand(UINT commandId, UINT notificationCode, bool hasControl);
	static void LogUiInteraction(UINT message, UINT controlId);
	static void LogUploadDecision(const char* decision, INT_PTR waitingUsers,
		INT_PTR openSlots, INT_PTR activeSlots);
#else
	static void Initialize() {}
	static void Shutdown() {}
	static void Sample() {}
	static void LogEvent(const char*, const char*, uint64 = 0, uint64 = 0, uint64 = 0) {}
	static void LogUiCommand(UINT, UINT, bool) {}
	static void LogUiInteraction(UINT, UINT) {}
	static void LogUploadDecision(const char*, INT_PTR, INT_PTR, INT_PTR) {}
#endif
};
