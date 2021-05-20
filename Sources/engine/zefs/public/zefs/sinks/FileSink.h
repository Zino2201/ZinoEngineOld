#pragma once

#include "logger/Logger.h"
#include "logger/Sink.h"
#include "zefs/FileStream.h"
#include <ostream>

namespace ze::filesystem
{

/**
 * A sink that supports writing to a file
 */
class ZEFS_API FileSink : public logger::Sink
{
public:
	FileSink(const std::string& in_name,
		const std::string& in_filename);

	void log(const logger::Message& InMessage) override;
private:
	std::string filename;
	FileOStream stream;
};

}