#include "EngineCore.h"
#include "Logger/Sinks/FileSink.h"
#include "FileSystem/ZFS.h"

namespace ZE::Logger::Sinks
{

CFileSink::CFileSink(const std::string& InName,
	const std::string& InFileName) : 
	CSink(InName), FileStream(InFileName), Stream(&FileStream) {}

void CFileSink::Log(const SMessage& InMessage)
{
	std::string Msg = Format(InMessage);

	Stream << Msg;
	Stream.flush();
}

}