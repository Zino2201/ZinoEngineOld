#pragma once

#include "EngineCore.h"
#include "Delegates/Delegate.h"

namespace ZE
{

enum class EAppOS
{
	Windows,
	Mac,
	Linux,
	FreeBSD,
	Android,
	Unknown
};

/**
 * Base abstract class for applications
 * Provides a main loop to operate on
 */
class CORE_API CApp
{
public:
	CApp(const int& InArgc, const char** InArgv);
	virtual ~CApp() = default;

	/** Run the main loop, will return only when Exit is called */
	int Run();
	void Exit(const int& InErrCode);

	/** Process all pending events, called by Run() loop by default but can be called if needed */
	virtual void ProcessEvents() = 0;

	ZE_FORCEINLINE static CApp* GetCurrentApp() { return CurrentApp; }
protected:
	virtual void Loop() = 0;
private:
	inline static CApp* CurrentApp = nullptr;
	int ErrCode;
protected:
	bool bRun;
};

namespace App
{

CORE_API void Exit(const int& InErrCode);

ZE_FORCEINLINE constexpr EAppOS GetOS()
{
#if ZE_PLATFORM(WINDOWS)
	return EAppOS::Windows;
#elif ZE_PLATFORM(OSX)
	return EAppOS::Mac;
#elif ZE_PLATFORM(LINUX)
	return EAppOS::Linux;
#elif ZE_PLATFORM(FREEBSD)
	return EAppOS::FreeBSD;
#elif ZE_PLATFORM(ANDROID)
	return EAppOS::Android;
#else
	return EAppOS::Unknown;
#endif
}

}

}