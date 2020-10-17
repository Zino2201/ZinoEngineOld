#pragma once

#include "EngineCore.h"
#include "FileSystem.h"
#include <istream>
#include <ostream>

namespace ze::filesystem
{

/**
 * Input filestream
 */
class ZEFS_API FileIStream : public std::istream
{
public:
	FileIStream(const std::filesystem::path& path,
		const FileReadFlags& flags = FileReadFlagBits::None);
	~FileIStream() { delete rdbuf(); }
};

/**
 * Output filestream
 */
class ZEFS_API FileOStream : public std::ostream
{
public:
	FileOStream(const std::filesystem::path& path,
		const FileWriteFlags& flags = FileWriteFlagBits::None);
	~FileOStream() { delete rdbuf(); }
};

}