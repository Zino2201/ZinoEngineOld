#pragma once

#include <filesystem>

struct Header;

class Writer
{
public:
	Writer(const Header& in_header, const std::filesystem::path& in_out_dir);
private:
	void write_h();
	void write_cpp();
private:
	const Header& header;
	std::filesystem::path out_dir;
};