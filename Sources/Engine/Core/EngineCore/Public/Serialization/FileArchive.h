#pragma once

#include "Serialization/Archive.h"
#include "FileSystem/FileSystem.h"

namespace ZE::Serialization
{

/**
 * A file archiver
 * Will own the specified IFile
 */
class ENGINECORE_API CFileArchive : public IArchive
{
public:
	CFileArchive(TOwnerPtr<ZE::FileSystem::IFile> InFile);

	void Serialize(const void* InData, const uint64_t& InSize) override;
	void Deserialize(void* InData, const uint64_t& InSize) override;
	void Flush() override;
private:
	std::unique_ptr<ZE::FileSystem::IFile> File;
};

}