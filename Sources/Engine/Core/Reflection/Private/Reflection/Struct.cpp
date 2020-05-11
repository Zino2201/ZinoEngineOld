#include "Reflection/Struct.h"

namespace ZE::Refl
{

namespace Internal
{

CStruct* GetStructByName(const char* InName)
{
	return CStruct::Get(InName);
}

}

CStruct* CStruct::Get(const char* InName)
{
	for (auto& Struct : Structs)
	{
		if(std::strcmp(Struct->GetName(), InName) == 0)
			return Struct;
	}

	return nullptr;
}

void CStruct::AddStruct(CStruct* InStruct)
{
	Structs.push_back(InStruct);

	// TODO: Make a better system
	for(auto& Struct : Structs)
	{
		for (auto It = Struct->Refl_ParentsWaitingAdd.begin(); 
			It != Struct->Refl_ParentsWaitingAdd.end();)
		{
			auto Parent = CStruct::Get(*It);
			if (Parent)
			{
				Struct->AddParent(Parent);
				It = Struct->Refl_ParentsWaitingAdd.erase(It);
			}
			else
				++It;
		}
	}
}

void CStruct::AddParent(CStruct* InParent)
{
	Parents.push_back(InParent);
}

bool CStruct::IsDerivedFrom(CStruct* InParent) const
{
	if (this == InParent)
		return true;

	for (const auto& Parent : Parents)
	{
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

}