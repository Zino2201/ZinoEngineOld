#include "Reflection/Type.h"

namespace ZE::Refl
{

__attribute__((__init_priority__(3000))) std::vector<std::unique_ptr<CType>> Types;

const CType* RegisterType(TOwnerPtr<CType> InType)
{
	bool bDoesTypeExist = GetTypeByName(InType->GetName());
	if(bDoesTypeExist)
		ZE::Logger::Fatal("Error! Type {} is already registered",
			InType->GetName());

	Types.emplace_back(InType);
	return Types.back().get();
}

const CType* GetTypeByName(const std::string& InName)
{
	for (const auto& Type : Types)
	{
		if (std::strcmp(Type->GetName(), InName.data()) == 0)
			return Type.get();
	}

	return nullptr;
}

}