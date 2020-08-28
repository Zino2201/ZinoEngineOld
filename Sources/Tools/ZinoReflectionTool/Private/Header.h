#pragma once

#include "Class.h"

/**
 * Represents a C++ header
 */
class CHeader
{
	friend struct SHeaderHash;

public:
	CHeader(const std::string_view& InModule, const std::filesystem::path& InPath,
		const std::string& InFilename) : 
		Module(InModule), Path(InPath), Filename(InFilename) {}

	CStruct& AddStruct(const std::string& InName, const std::string& InNamespace,
		const size_t& InDeclLine);
	CClass& AddClass(const std::string& InName, const std::string& InNamespace,
		const size_t& InDeclLine);

	const std::vector<CStruct>& GetStructs() const { return Structs; }
	const std::vector<CClass>& GetClasses() const { return Classes; }
	const std::string& GetFilename() const { return Filename; }
	const std::string& GetModule() const { return Module; }
	const std::filesystem::path& GetPath() const { return Path; }
	bool HasReflData() const { return !Structs.empty() || !Classes.empty(); }
private:
	std::string Module;
	std::string Filename;
	std::filesystem::path Path;
	std::vector<CStruct> Structs;
	std::vector<CClass> Classes;
};