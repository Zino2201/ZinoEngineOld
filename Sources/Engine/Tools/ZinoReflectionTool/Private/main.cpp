#include <iostream>
#include "ZRT.h"
#include <cstdarg>
#include <filesystem>
#include <chrono>
#include "Header.h"
#include "Parser.h"
#include "Writer.h"
#include <fstream>
#include <sys/stat.h>

void Fatal(std::string_view InMessage, ...)
{
	va_list InArgs;
	va_start(InArgs, InMessage);

	char PrintfBuffer[2048];
	vsprintf(PrintfBuffer, InMessage.data(), InArgs);
	std::cerr << PrintfBuffer;
	std::cerr << std::endl;

	va_end(InArgs);

	exit(-1);
}

std::string_view ParseCommandLineArg(const std::string_view& InArg)
{
	size_t EqId = InArg.find('=');
	return InArg.substr(EqId + 1, InArg.size() - EqId);
}

using HighResClock = std::chrono::high_resolution_clock;

// stat
#ifdef _WIN64
#define STAT _stat
using STAT_TIME = __time64_t;
#else
#define STAT stat
using STAT_TIME = __int64_t;
#endif

int main(int argc, char** argv)
{
	std::ios::sync_with_stdio(false);

	if(argc < 2)
	{
		Fatal("Invalid syntax: ZinoReflectionTool.exe -SrcDir= -OutDir= ZRTFILES");
	}

	std::cout << "ZinoReflectionTool now running\n";

	/**
	 * Register defaults type to type database
	 */
	for(const auto& AcceptedType : GAcceptedTypes)
		CTypeDatabase::Get().RegisterType(AcceptedType.data());
		
	/**
	 * Load ZRT files and parse them
	 */
	int ZrtFileCount = argc;
	for(int i = 4; i < ZrtFileCount; ++i)
	{
		std::vector<std::string> Lines = ReadTextFile(argv[i]);
		for(const auto& Line : Lines)
			CParser Parser(nullptr, Line, true);
	}

	/**
	 * Register 
	 */

	auto Start = HighResClock::now();

	/** Parse command line arguments */
	std::string_view ModuleName = ParseCommandLineArg(argv[1]);
	std::string_view SrcDir = ParseCommandLineArg(argv[2]);
	std::string_view OutDir = ParseCommandLineArg(argv[3]);

	/**
	 * Iterate over headers and parse them
	 */
	std::vector<CHeader> Headers;
	for(const auto& Entry : std::filesystem::recursive_directory_iterator(SrcDir))
	{
		std::filesystem::path Path = Entry.path();

		if(Path.extension() == ".h")
		{
			/**
			 * Check if file was modified
			 */
			std::filesystem::path DateFile = OutDir;
			DateFile += Path.filename();
			DateFile += ".date";
			if(std::filesystem::exists(DateFile))
			{
				std::ifstream File(DateFile);
				STAT_TIME Time;
				File >> Time;

				struct STAT WriteTime;
				STAT(Path.string().c_str(), &WriteTime);
				if(Time == WriteTime.st_mtime)
					continue;
				
				File.close();
			}
			
			std::ofstream File(DateFile);
			struct STAT WriteTime;
			STAT(Path.string().c_str(), &WriteTime);
			File << WriteTime.st_mtime;
			File.close();

			Headers.emplace_back(ModuleName, Path, Path.filename().string());
			CParser Parser(&Headers.back(), Path.string());
		}
	}
	
	/**
	 * Validate all headers classes & structures
	 */
	for(const auto& Header : Headers)
	{
		/**
		 * For each struct/class property check if the type is valid
		 */
		for(const auto& Struct : Header.GetStructs())
		{
			for(const auto& Property : Struct.GetProperties())
			{
				if (!CTypeDatabase::Get().HasType(Property.Type))
				{
					const auto& Known = GKnownUnsupportedTypes.find(Property.Type);
					if(Known != GKnownUnsupportedTypes.end())
					{
						Fatal("%s: error: Unsupported type \"%s\" of property \"%s\", use \"%s\" (%s)",
							Header.GetFilename().c_str(),
							Property.Type.c_str(),
							Property.Name.c_str(),
							Known->second.data(),
							Struct.GetName().c_str());
					}

					Fatal("%s: error: Unknown type \"%s\" of property \"%s\" (%s)",
						Header.GetFilename().c_str(),
						Property.Type.c_str(),
						Property.Name.c_str(),
						Struct.GetName().c_str());
				}
			}
		}

		for(const auto& Class : Header.GetClasses())
		{
			for (const auto& Property : Class.GetProperties())
			{
				if (!CTypeDatabase::Get().HasType(Property.Type))
				{
					const auto& Known = GKnownUnsupportedTypes.find(Property.Type);
					if (Known != GKnownUnsupportedTypes.end())
					{
						Fatal("%s: error: Unsupported type \"%s\" of property \"%s\", use \"%s\" (%s)",
							Header.GetFilename().c_str(),
							Property.Type.c_str(),
							Property.Name.c_str(),
							Known->second.data(),
							Class.GetName().c_str());
					}

					Fatal("%s: Unknown type \"%s\" of property \"%s\" (%s)",
						Header.GetFilename().c_str(),
						Property.Type.c_str(),
						Property.Name.c_str(),
						Class.GetName().c_str());
				}
			}
		}
	}

	/** Now write headers */
	for(const auto& Header : Headers)
	{
		if(Header.HasReflData())
		{
			Writer::Write(OutDir, Header);
		}
	}

	auto End = HighResClock::now();
	std::cout << "Reflection code generated in "
		<< std::chrono::duration_cast<std::chrono::duration<double>>(End - Start).count()
		<< " seconds\n";
	return 0;
}