#include "Header.h"

CStruct& CHeader::AddStruct(const std::string& InName, const std::string& InNamespace,
	const size_t& InDeclLine)
{
	Structs.emplace_back(InName, InNamespace, InDeclLine);
	return Structs.back();
}

CClass& CHeader::AddClass(const std::string& InName, const std::string& InNamespace,
	const size_t& InDeclLine)
{
	Classes.emplace_back(InName, InNamespace, InDeclLine);
	return Classes.back();
}