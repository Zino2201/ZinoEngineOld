#include "Struct.h"

void CStruct::AddProperty(const std::string& InName, const std::string& InType,
	const EAccess& InAccess,
	const EPropertyFlags& InFlags,
	const EPropertyAttributes& InAttribs)
{
	Properties.emplace_back(InName, InType, InAccess, InFlags, InAttribs);
}

void CStruct::AddParent(const std::string& InParent)
{
	Parents.emplace_back(InParent);
}