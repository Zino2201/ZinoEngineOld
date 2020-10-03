#pragma once

#include "Type.h"
#include <any>

namespace ZE::Refl
{

/**
 * A C++ enum
 */
class REFLECTION_API CEnum : public CType
{
public:
	CEnum(const char* InName,
		const uint64_t& InSize) : CType(InName, InSize), UnderlyingType() {}

	void AddValue(const std::string& InName, const std::any& InValue);
	void SetUnderlyingType(const TLazyTypePtr<CType>& InUnderlyingType) { UnderlyingType = InUnderlyingType; }

	const CType* GetUnderlyingType() { return UnderlyingType.Get(); }
private:
	std::vector<std::pair<std::string, std::any>> Values;
	TLazyTypePtr<CType> UnderlyingType;
};

/**
 * Register an enumeration
 */
REFLECTION_API void RegisterEnum(CEnum* InEnum);


}