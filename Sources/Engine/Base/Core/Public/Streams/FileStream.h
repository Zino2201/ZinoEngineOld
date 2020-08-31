#pragma once

#include <ios>
#include "FileSystem/ZFS.h"

namespace ZE::Streams
{

/**
 * std::fstream equivalent using ZFS
 */
class CFileStream : public std::streambuf
{
public:
	CFileStream(const std::string_view& InPath, const bool& bWrite,
		const ZE::FileSystem::EFileReadFlags& InReadFlags,
		const ZE::FileSystem::EFileWriteFlags& InWriteFlags) 
	{
		if(bWrite)
			File = std::unique_ptr<ZE::FileSystem::IFile>(ZE::FileSystem::Write(InPath, InWriteFlags));
		else
			File = std::unique_ptr<ZE::FileSystem::IFile>(ZE::FileSystem::Read(InPath, InReadFlags));
	
		verify(File);
	}

	operator bool() const { return !!File; }
protected:
	int sync() override
	{
		verify(File);

		File->Flush();
		return 0;
	}
	
	std::streamsize xsputn(const char_type* InChar, std::streamsize InSize) override
	{
		verify(File);

		File->Write(reinterpret_cast<const uint8_t*>(InChar), InSize);
		return InSize;
	}

	std::streamsize xsgetn(char_type* InChar, std::streamsize InSize) override 
	{
		verify(File);

		File->Read(reinterpret_cast<uint8_t*>(InChar), InSize);
		return InSize;
	}

	int_type overflow(int_type Char) override
	{
		verify(File);
		
		File->Write(reinterpret_cast<const uint8_t*>(&Char), sizeof(int_type));
		return Char;
	}

	int_type underflow() override
	{
		verify(File);
		
		int_type Char = 0;
		File->Read(reinterpret_cast<uint8_t*>(&Char), sizeof(int_type));
		return Char;
	}
private:
	std::unique_ptr<ZE::FileSystem::IFile> File;
};

class CIFileStream : public CFileStream
{
public:
	CIFileStream(const std::string_view& InPath, 
		const ZE::FileSystem::EFileReadFlags& InReadFlags = ZE::FileSystem::EFileReadFlagBits::None) :
		CFileStream(InPath, false, InReadFlags, ZE::FileSystem::EFileWriteFlagBits::None) {}
};

class COFileStream : public CFileStream
{
public:
	COFileStream(const std::string_view& InPath, 
		const ZE::FileSystem::EFileWriteFlags& InWriteFlags = ZE::FileSystem::EFileWriteFlagBits::None) :
		CFileStream(InPath, true, ZE::FileSystem::EFileReadFlagBits::None,
			InWriteFlags) {}
};

}