#include "Serialization/FileArchive.h"

namespace ZE::Serialization
{

CFileArchive::CFileArchive(TOwnerPtr<ZE::FileSystem::IFile> InFile) :
	File(std::unique_ptr<ZE::FileSystem::IFile>(InFile)) 
{
	bIsSaving = !InFile->IsReading();
	bIsBinary = InFile->IsBinary();
}

void CFileArchive::Serialize(const void* InData, const uint64_t& InSize)
{
	std::string th = reinterpret_cast<const char*>(InData);
	File->Write(reinterpret_cast<const uint8_t*>(InData), InSize);
}

void CFileArchive::Deserialize(void* InData, const uint64_t& InSize)
{
	File->Read(reinterpret_cast<uint8_t*>(InData), InSize);
}

void CFileArchive::Flush()
{
	File->Flush();
}

}