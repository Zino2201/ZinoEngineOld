#pragma once

#include "FileSystem/FileUtils.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/ZFS.h"
#include <filesystem>

namespace FS = ZE::FileSystem;

namespace ZE
{

namespace FileUtils
{

std::string GetCurrentWorkingDirectory()
{
	return std::filesystem::current_path().string();
}

std::string GetShadersDirectory()
{
	return std::filesystem::current_path().string() + "/Shaders/";
}

void ReadTextFile(const std::string_view& InPath,
	std::string& InString)
{
	TOwnerPtr<FS::IFile> File = FS::Read(InPath); 

	int64_t Size = File->GetSize();
	
	std::vector<uint8_t> Array;
	Array.resize(Size / sizeof(uint8_t));
	
	File->Read(Array.data(), Size);

	InString = std::string(Array.begin(), Array.end());

	delete File;
}
}

namespace PathUtils
{

std::string GetFilename(const std::string_view& InPath)
{
	return std::filesystem::path(InPath).filename().string();
}

}

}