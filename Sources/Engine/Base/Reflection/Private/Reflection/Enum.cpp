#include "Reflection/Enum.h"

namespace ZE::Refl
{

std::vector<CEnum*> Enums;

void RegisterEnum(CEnum* InEnum)
{
	Enums.emplace_back(InEnum);
}

void CEnum::AddValue(const std::string& InName, const std::any& InValue)
{
	Values.emplace_back(InName, InValue);
}

}