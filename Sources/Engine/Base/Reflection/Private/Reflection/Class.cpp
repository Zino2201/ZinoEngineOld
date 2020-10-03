#include "Reflection/Class.h"

namespace ZE::Refl
{

__attribute__((__init_priority__(2998))) std::vector<const CClass*> Classes;

void RegisterClass(const CClass* InClass)
{
	Classes.emplace_back(InClass);
}

const CClass* GetClassByName(const std::string& InName)
{
	for (const auto& Class : Classes)
	{
		if (std::strcmp(Class->GetName(), InName.data()) == 0)
			return Class;
	}

	return nullptr;
}

const std::vector<const CClass*> GetClasses()
{
	return Classes;
}


}