#include "EngineCore.h"
#include "ZEFS/ZEFS.h"
#include <map>

namespace ZE::FileSystem
{

struct SFSEntry
{
	std::string Name;
	uint8_t Priority;
	std::string Alias;

	SFSEntry() : Priority(0), Alias() {}
	SFSEntry(const std::string& InName, const std::string& InAlias,
		const uint8_t& InPriority) : Name(InName), Priority(InPriority), Alias(InAlias) {}

	bool operator<(const SFSEntry& InOther) const
	{
		return Priority < InOther.Priority;
	}
};

/** Sorted map of all file systems */
std::map<SFSEntry, std::unique_ptr<IFileSystem>> Filesystems;

/** Ref to the writer file system, if specified.
 * Defaults to the first added that is not read only */
IFileSystem* WriteFS = nullptr;

/**
 * Get filesystems that has a matching alias inside the path
 */
std::vector<IFileSystem*> GetFileSystemsMatchingAlias(const std::string_view& InPath)
{
	std::vector<IFileSystem*> FileSystems;
	FileSystems.reserve(2);

	for (const auto& [Entry, FS] : Filesystems)
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

	for (const auto& FS : FileSystems)
	{
		if (InLambda(FS))
			return true;
	}

	return false;
}

template<typename Ret, bool bWriteOp = false, typename Lambda>
Ret ExecutePtr(const std::string_view& InPath, Lambda&& InLambda)
{
	std::vector<IFileSystem*> FileSystems = GetFileSystemsMatchingAlias(InPath);

	if constexpr (bWriteOp)
	{
		if (WriteFS)
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

TOwnerPtr<std::streambuf> Read(const std::string_view& InPath,
	const EFileReadFlags& InReadFlags)
{
	return ExecutePtr<TOwnerPtr<std::streambuf>>(InPath,
		[InPath, InReadFlags](IFileSystem* InFS) -> TOwnerPtr<std::streambuf>
		{
			return InFS->Read(InPath, InReadFlags);
		});
}

TOwnerPtr<std::streambuf> Write(const std::string_view& InPath,
	const EFileWriteFlags& InWriteFlags)
{
	return ExecutePtr<TOwnerPtr<std::streambuf>, true>(InPath,
		[InPath, InWriteFlags](IFileSystem* InFS) -> TOwnerPtr<std::streambuf>
		{
			if (InFS->Exists(InPath))
			{
				if (!(InWriteFlags & EFileWriteFlagBits::ReplaceExisting))
				{
					ZE::Logger::Error("Can't write to file {}: file already exists",
						InPath.data());
					return nullptr;
				}
			}

			return InFS->Write(InPath, InWriteFlags);
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

bool IsDirectory(const std::string_view& InPath)
{
	return Execute(InPath,
		[InPath](IFileSystem* InFS) -> bool
		{
			return InFS->IsDirectory(InPath);
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

void SetWriteFS(IFileSystem& InFS)
{
	must(!InFS.IsReadOnly());
	
	WriteFS = &InFS;
}

IFileSystem& AddFileSystem(const std::string& InName, const std::string& InAlias,
	const uint8_t& InPriority, TOwnerPtr<IFileSystem> InFS)
{
	Filesystems.insert({ SFSEntry(InName, InAlias, InPriority),
		std::unique_ptr<IFileSystem>(InFS) });

	ZE::Logger::Info("Added new filesystem {} (alias {}/{})", InName,
		InAlias.c_str(),
		InPriority);

	return *InFS;
}

}