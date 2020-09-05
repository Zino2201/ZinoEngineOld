#pragma once

#include "Logger/Logger.h"
#include "Logger/Sink.h"
#include "ZEFS/FileStream.h"
#include <ostream>

namespace ZE::FileSystem
{

/**
 * A sink that supports writing to a file
 */
class ZEFS_API CFileSink : public ZE::Logger::CSink
{
public:
	CFileSink(const std::string& InName,
		const std::string& InFileName);

	void Log(const ZE::Logger::SMessage& InMessage) override;
private:
	std::string Filename;
	COFileStream FileStream;
};

}