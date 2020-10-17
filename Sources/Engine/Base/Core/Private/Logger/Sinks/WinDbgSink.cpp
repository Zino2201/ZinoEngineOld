#include "EngineCore.h"
#include "Logger/Sinks/WinDbgSink.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace ze::logger
{

void WinDbgSink::log(const Message& message)
{
#if ZE_PLATFORM(WINDOWS)
	std::string msg = format(message);

	OutputDebugStringA(msg.c_str());
#endif
}

}