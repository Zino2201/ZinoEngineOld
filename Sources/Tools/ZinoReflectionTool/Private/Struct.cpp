#include "Struct.h"

void CStruct::AddProperty(const std::string& InName, const std::string& InType,
	const EAccess& InAccess,
	const EPropertyFlags& InFlags)
{
	Properties.emplace_back(InName, InType, InAccess, InFlags);
}

void CStruct::AddParent(const std::string& InParent)
{
	Parents.emplace_back(InParent);
}