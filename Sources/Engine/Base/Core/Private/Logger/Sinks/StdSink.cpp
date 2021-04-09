#include "EngineCore.h"
#include "Logger/Sinks/StdSink.h"
#include <iostream>
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincon.h>
#endif

namespace ze::logger
{

void StdSink::log(const Message& message)
{
#if ZE_PLATFORM(WINDOWS)
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    
    switch(message.severity)
    {
    case SeverityFlagBits::Warn:
        SetConsoleTextAttribute(console, 14);
        break;
    case SeverityFlagBits::Error:
        SetConsoleTextAttribute(console, 12);
        break;
    case SeverityFlagBits::Fatal:
        SetConsoleTextAttribute(console, BACKGROUND_RED);
        break;
    default:
        break;
    }
    std::cout << format(message);
    SetConsoleTextAttribute(console, 7);
#else
    std::cout << format(message);
#endif
}

}