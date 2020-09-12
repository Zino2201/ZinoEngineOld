#include "ZEFS/FileStream.h"
#include "ZEFS/ZEFS.h"

namespace ZE::FileSystem
{

CIFileStream::CIFileStream(const std::filesystem::path& InPath,
	const ZE::FileSystem::EFileReadFlags& InReadFlags) :
	std::istream(Read(InPath, InReadFlags)) {}

COFileStream::COFileStream(const std::filesystem::path& InPath,
	const ZE::FileSystem::EFileWriteFlags& InWriteFlags) :
	std::ostream(Write(InPath, InWriteFlags)) {}

}