#include "FileSystem/ZFS.h"

namespace ZE::FileSystem
{

#pragma region Utils funcs

std::vector<IFileSystem*> GetFileSystemsMatchingAlias(const std::string_view& InPath)
{
	std::vector<IFileSystem*> FileSystems;
	FileSystems.reserve(2);

	for (const auto& [Entry, FS] : CFileSystemManager::Get().GetFilesystems())
	{
		if (Entry.Alias == "/" ||
			InPath.rfind(Entry.Alias, 0) == 0)
		{
			FileSystems.emplace_back(FS.get());
		}
	}

	return FileSystems;
}

/**
 * Helper functions calling a lambda using filesystems 
 * gathered using GetFileSystemsMatchingAlias
 */
template<typename Lambda>
bool Execute(const std::string_view& InPath, Lambda&& InLambda)
{
	std::vector<IFileSystem*> FileSystems = GetFileSystemsMatchingAlias(InPath);

	for(const auto& FS : FileSystems)
	{
		if(InLambda(FS))
			return true;
	}

	return false;
}

template<typename Ret, bool bWriteOp = false, typename Lambda>
Ret ExecutePtr(const std::string_view& InPath, Lambda&& InLambda)
{
	std::vector<IFileSystem*> FileSystems = GetFileSystemsMatchingAlias(InPath);

	if constexpr(bWriteOp)
	{
		IFileSystem* WriteFS = CFileSystemManager::Get().GetWriteFS();

		if(WriteFS)
			return InLambda(WriteFS);
	}
	else
	{
		for (const auto& FS : FileSystems)
		{
			Ret Ptr = InLambda(FS);
			if (Ptr)
				return Ptr;
		}
	}

	return nullptr;
}

#pragma endregion

TOwnerPtr<IFile> Read(const std::string_view& InPath,
	const EFileReadFlags& InReadFlags)
{
	return ExecutePtr<TOwnerPtr<IFile>>(InPath,
		[InPath, InReadFlags](IFileSystem* InFS) -> TOwnerPtr<IFile>
	{
		return InFS->Read(InPath, false);
	});
}

TOwnerPtr<IFile> Write(const std::string_view& InPath, 
	const EFileWriteFlags& InWriteFlags)
{
	return ExecutePtr<TOwnerPtr<IFile>, true>(InPath,
		[InPath, InWriteFlags](IFileSystem* InFS) -> TOwnerPtr<IFile>
	{
		if(InFS->Exists(InPath))
		{
			if (!(InWriteFlags & EFileWriteFlagBits::ReplaceExisting))
			{
				ZE::Logger::Error("Can't write to file {}: file already exists",
					InPath.data());
				return nullptr;
			}
		}

		bool bIsBinary = false;
		if(InWriteFlags & EFileWriteFlagBits::Binary)
			bIsBinary = true;

		return InFS->Write(InPath, bIsBinary);
	});
}

bool Exists(const std::string_view& InPath)
{
	return Execute(InPath,
		[InPath](IFileSystem* InFS) -> bool
		{
			return InFS->Exists(InPath);
		});
}

bool IterateDirectories(const std::string_view& InPath,
	const TDirectoryIterator& InIt)
{
	return Execute(InPath,
		[InPath, InIt](IFileSystem* InFS) -> bool
	{
		return InFS->IterateDirectories(InPath, InIt);
	});
}

void SetWriteFS(IFileSystem* InFS)
{
	CFileSystemManager::Get().SetWriteFS(InFS);
}

IFileSystem* GetFileSystemByName(const std::string_view& InName)
{
	for (const auto& [Entry, FS] : CFileSystemManager::Get().GetFilesystems())
	{
		if(Entry.Name == InName)
			return FS.get();
	}

	return nullptr;
}

}