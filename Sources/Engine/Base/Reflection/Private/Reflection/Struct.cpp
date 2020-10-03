#include "Reflection/Struct.h"

namespace ZE::Refl
{

__attribute__((__init_priority__(2999))) std::vector<const CStruct*> Structs;

void RegisterStruct(const CStruct* InStruct)
{
	Structs.emplace_back(InStruct);
}

const CStruct* GetStructByName(const std::string& InName)
{
	for (const auto& Struct : Structs)
	{
		if (std::strcmp(Struct->GetName(), InName.data()) == 0)
			return Struct;
	}

	return nullptr;
}


void CStruct::AddParent(const std::string& InParent)
{
	Parents.emplace_back(InParent);
}

bool CStruct::IsDerivedFrom(const CStruct* InParent) const
{
	if (this == InParent)
		return true;

	for (const auto& ParentPtr : Parents)
	{
		const CStruct* Parent = ParentPtr.Get();

		if (Parent == InParent)
			return true;
		else
		{
			bool bRet = Parent->IsDerivedFrom(InParent);
			if (bRet)
				return true;
		}
	}

	return false;
}

const std::vector<const CStruct*> GetStructs()
{
	return Structs;
}

}