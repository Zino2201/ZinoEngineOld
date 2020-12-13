#include "EngineCore.h"
#include "Logger/Sinks/StdSink.h"
#include <iostream>

namespace ze::logger
{

void StdSink::log(const Message& message)
{
    std::cout << format(message) << std::endl;
}

}