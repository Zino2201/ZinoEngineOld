#include "Reflection/Class.h"

namespace ZE::Refl
{

namespace Internal
{

TNonOwningPtr<CClass> GetClassByName(const char* InName)
{
	// TODO: implement
	must(false);
	return nullptr;
}
}

TNonOwningPtr<CClass> CClass::Get(const char* InName)
{
	for (auto& Class : Classes)
	{
		if (std::strcmp(Class->GetName(), InName) == 0)
			return Class;
	}

	return nullptr;
}

void CClass::AddClass(const TNonOwningPtr<CClass>& InClass)
{
	Classes.push_back(InClass);
}

}