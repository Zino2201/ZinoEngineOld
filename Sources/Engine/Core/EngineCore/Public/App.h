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

ENGINECORE_API void Exit(const int& InErrCode);

ZE_FORCEINLINE constexpr EAppOS GetOS()
{
#ifdef ZE_WIN64
	return EAppOS::Windows;
#elif ZE_OSX
	return EAppOS::Mac;
#elif ZE_LINUX
	return EAppOS::Linux;
#elif ZE_FREEBSD
	return EAppOS::FreeBSD;
#else
	return EAppOS::Unknown;
#endif
}

}

}