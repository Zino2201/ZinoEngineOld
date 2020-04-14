#pragma once

#include "EngineCore.h"
#include <fstream>
#include <string>

namespace ZE
{

DECLARE_LOG_CATEGORY(IOUtils);

namespace IOUtils
{
	static std::vector<uint8_t> ReadBinaryFile(const std::string_view& InFilename)
	{
		std::ifstream File(InFilename.data(), std::ios::ate | std::ios::binary);

		if (!File.is_open())
		{
			LOG(ELogSeverity::Error, IOUtils, "Failed to read file %s", InFilename.data());
			return {};
		}

		size_t FileSize = static_cast<size_t>(File.tellg());
		std::vector<uint8_t> Buffer(FileSize);
		File.seekg(0);
		File.read(reinterpret_cast<char*>(Buffer.data()), FileSize);
		File.close();

		return Buffer;
	}

	static std::string ReadTextFile(const std::string_view& InFilename)
	{
		std::string Line, Text;
		std::ifstream File(InFilename.data());
		if (!File.is_open())
			LOG(ELogSeverity::Error, IOUtils, "Failed to read file %s", InFilename.data());

		while (std::getline(File, Line))
		{
			Text += Line + "\n";
		}

		File.close();

		return Text;
	}
}

} /* namespace ZE */