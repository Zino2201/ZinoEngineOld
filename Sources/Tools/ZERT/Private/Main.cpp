#include "ZERT.h"
#include <iostream>
#include <cstdarg>
#include <string>
#include <sys/stat.h>
#include "Type.h"
#include "Parser.h"
#include <fstream>
#include "Header.h"
#include "Writer.h"

std::string_view parse_command_line_arg(const std::string_view& arg)
{
	size_t id = arg.find('=');
	return arg.substr(id + 1, arg.size() - id);
}

void fatal(std::string_view InMessage, ...)
{
	va_list InArgs;
	va_start(InArgs, InMessage);

	char PrintfBuffer[2048];
	vsprintf(PrintfBuffer, InMessage.data(), InArgs);
	std::cout << PrintfBuffer;
	std::cout << std::endl;

	va_end(InArgs);

	exit(-1);
}

// stat
#ifdef _WIN64
#define STAT _stat
using stat_time_t = __time64_t;
#else
#define STAT stat
using stat_time_t = __int64_t;
#endif

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fatal("Invalid syntax: ZERT.exe -SrcDir= -OutDir= ZRTFILES");
	}

	/**
	 * Register primitives type to type database
	 */
	for(const auto& type : primitive_types)
		typedb_register(Type(ReflType::Primitive, type));
	
	/**
	 * Load ZRT files and parse them
	 */
	int ZrtFileCount = argc;
	for(int i = 4; i < ZrtFileCount; ++i)
	{
		Header headesr("", "");
		std::vector<std::string> headers = read_file_lines(argv[i]);
		for(const auto& header : headers)
			Parser parser(headesr, read_text_file(header), true);
	}

	auto start = std::chrono::high_resolution_clock::now();

	/** Parse command line arguments */
	std::string_view module_name = parse_command_line_arg(argv[1]);
	std::string_view src_dir = parse_command_line_arg(argv[2]);
	std::string_view out_dir = parse_command_line_arg(argv[3]);

	std::cout << "Generating reflection data for module " << module_name << "...\n";

	/**
	 * Iterate over headers and parse them
	 */
	std::vector<Header> headers;
	for(const auto& entry : std::filesystem::recursive_directory_iterator(src_dir))
	{
		std::filesystem::path path = entry.path();

		if(path.extension() == ".h")
		{
			/**
			 * Check if file was modified
			 */
			std::filesystem::path datefile = out_dir;
			datefile += path.filename();
			datefile += ".date";
			if(std::filesystem::exists(datefile))
			{
				std::ifstream file(datefile);
				stat_time_t time;
				file >> time;

				struct STAT write_time;
				STAT(path.string().c_str(), &write_time);
				if(time == write_time.st_mtime)
					continue;
				
				file.close();
			}
			
			std::ofstream file(datefile);
			struct STAT write_time;
			STAT(path.string().c_str(), &write_time);
			file << write_time.st_mtime;
			file.close();

			Header& header = headers.emplace_back(std::string(module_name), path);
			Parser parser(header, read_text_file(path.string()), false);
		}
	}

	for(const auto& header : headers)
	{
		if(header.has_refl_data())
			Writer writer(header, out_dir);
	}

	return 0;
}