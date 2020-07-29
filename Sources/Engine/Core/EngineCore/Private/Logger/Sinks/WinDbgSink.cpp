#include "Logger/Sinks/WinDbgSink.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace ZE::Logger::Sinks
{

void CWinDbgSink::Log(const SMessage& InMessage)
{
	std::string Msg = Format(InMessage);

#ifdef _WIN32
	OutputDebugStringA(Msg.c_str());
#endif
}

}