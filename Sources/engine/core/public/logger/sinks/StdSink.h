#pragma once

#include "logger/Sink.h"

namespace ze::logger
{

/**
 * Std sink (std::cout)
 */
class CORE_API StdSink : public Sink
{
public:
    StdSink(const std::string& name) : Sink(name) {}

    void log(const Message& message) override;
};

}