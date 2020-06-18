#include "Serialization/FileArchive.h"

namespace ZE::Serialization
{

CFileArchive::CFileArchive(TOwnerPtr<ZE::FileSystem::IFile> InFile) :
	File(std::unique_ptr<ZE::FileSystem::IFile>(InFile)) 
{
	bIsSaving = !InFile->IsReading();
}

void CFileArchive::Serialize(const void* InData, const uint64_t& InSize)
{
	File->Write(reinterpret_cast<const uint8_t*>(InData), InSize);
}

void CFileArchive::Deserialize(void* InData, const uint64_t& InSize)
{
	File->Read(reinterpret_cast<uint8_t*>(InData), InSize);
}

}