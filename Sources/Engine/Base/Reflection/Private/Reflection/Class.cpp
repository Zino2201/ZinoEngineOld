#include "Reflection/Class.h"

namespace ZE::Refl
{

namespace Internal
{

CClass* GetClassByName(const char* InName)
{
	return CClass::Get(InName);
}
}

CClass* CClass::Get(const char* InName)
{
	for (auto& Class : Classes)
	{
		if (std::strcmp(Class->GetName(), InName) == 0)
			return Class;
	}

	return nullptr;
}

void CClass::AddClass(CClass* InClass)
{
	Classes.push_back(InClass);
}

}