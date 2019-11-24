#pragma once

#include "Core/EngineCore.h"
#include <fstream>

namespace IOUtils
{
	static std::vector<uint8_t> ReadFile(const std::string& InFilename)
	{
		std::ifstream File(InFilename, std::ios::ate | std::ios::binary);

		if (!File.is_open())
		{
			LOG(ELogSeverity::Error, "Failed to read file %s", InFilename.c_str())
			return {};
		}

		size_t FileSize = static_cast<size_t>(File.tellg());
		std::vector<uint8_t> Buffer(FileSize);
		File.seekg(0);
		File.read(reinterpret_cast<char*>(Buffer.data()), FileSize);
		File.close();

		return Buffer;
	}
}