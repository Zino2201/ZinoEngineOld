#include "Logger/Sinks/FileSink.h"
#include "FileSystem/ZFS.h"

namespace ZE::Logger::Sinks
{

CFileSink::CFileSink(const std::string& InName,
	const std::string& InFileName) : CSink(InName), Archive(ZE::FileSystem::Write(InFileName)) {}

void CFileSink::Log(const SMessage& InMessage)
{
	std::string Msg = Format(InMessage);

	Archive << Msg;
	Archive.Flush();
}

}