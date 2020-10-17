#include "ZEFS/FileStream.h"
#include "ZEFS/ZEFS.h"

namespace ze::filesystem
{

FileIStream::FileIStream(const std::filesystem::path& path,
	const FileReadFlags& flags) :
	std::istream(filesystem::read(path, flags)) {}

FileOStream::FileOStream(const std::filesystem::path& path,
	const FileWriteFlags& flags) :
	std::ostream(filesystem::write(path, flags)) {}

}