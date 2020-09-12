#pragma once

#include "EngineCore.h"
#include "FileSystem.h"
#include <istream>
#include <ostream>

namespace ZE::FileSystem
{

/**
 * Input filestream
 */
class ZEFS_API CIFileStream : public std::istream
{
public:
	CIFileStream(const std::filesystem::path& InPath,
		const ZE::FileSystem::EFileReadFlags& InReadFlags = ZE::FileSystem::EFileReadFlagBits::None);
	~CIFileStream() { delete rdbuf(); }
};

/**
 * Output filestream
 */
class ZEFS_API COFileStream : public std::ostream
{
public:
	COFileStream(const std::filesystem::path& InPath,
		const ZE::FileSystem::EFileWriteFlags& InWriteFlags = ZE::FileSystem::EFileWriteFlagBits::None);
	~COFileStream() { delete rdbuf(); }
};

}