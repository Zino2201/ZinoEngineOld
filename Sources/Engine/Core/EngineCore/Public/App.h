#pragma once

#include "EngineCore.h"

namespace ZE
{

enum class EAppOS
{
	Windows,
	Mac,
	Linux,
	FreeBSD,
	Unknown
};

/**
 * Base abstract class for applications
 * Provides a main loop to operate on
 */
class ENGINECORE_API CApp
{
public:
	CApp(const int& InArgc, const char** InArgv);
	virtual ~CApp() = default;

	/** Run the main loop, will return only when Exit is called */
	void Run();

	/** Process all pending events, called by Run() loop by default but can be called if needed */
	virtual void ProcessEvents() = 0;

	FORCEINLINE static CApp* GetCurrentApp() { return CurrentApp; }
protected:
	virtual void Loop() = 0;
private:
	inline static CApp* CurrentApp = nullptr;
protected:
	bool bRun;
};

namespace App
{

[[noreturn]] ENGINECORE_API void Exit(const int& InErrCode);

FORCEINLINE constexpr EAppOS GetOS()
{
#ifdef _WIN64
	return EAppOS::Windows;
#elif __APPLE__ || __MACH__
	return EAppOS::Mac;
#elif __LINUX__
	return EAppOS::Linux;
#elif __FREEBSD__
	return EAppOS::FreeBSD;
#else
	return EAppOS::Unknown;
#endif
}

}

}