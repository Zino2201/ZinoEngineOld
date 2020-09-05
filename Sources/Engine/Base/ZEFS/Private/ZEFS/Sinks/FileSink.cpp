#include "EngineCore.h"
#include "ZEFS/Sinks/FileSink.h"

namespace ZE::FileSystem
{

CFileSink::CFileSink(const std::string& InName,
	const std::string& InFileName) : 
	CSink(InName), FileStream(InFileName) {}

void CFileSink::Log(const ZE::Logger::SMessage& InMessage)
{
	std::string Msg = Format(InMessage);

	FileStream << Msg;
	FileStream.flush();
}

}