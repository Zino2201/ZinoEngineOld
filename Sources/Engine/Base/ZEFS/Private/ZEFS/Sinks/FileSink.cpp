#include "EngineCore.h"
#include "ZEFS/Sinks/FileSink.h"

namespace ze::filesystem
{

FileSink::FileSink(const std::string& in_name,
	const std::string& in_filename) : 
	Sink(in_name), stream(in_filename) {}

void FileSink::log(const logger::Message& message)
{
	std::string msg = format(message);

	stream << msg;
	stream.flush();
}

}