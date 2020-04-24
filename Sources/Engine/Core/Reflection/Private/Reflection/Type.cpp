#include "Reflection/Type.h"

namespace ZE::Refl
{

TNonOwningPtr<CType> CType::Get(const std::string_view& InName)
{
	for (auto& Type : Types)
	{
		if (std::strcmp(Type->Name, InName.data()) == 0)
			return Type.get();
	}

	return nullptr;
}

}