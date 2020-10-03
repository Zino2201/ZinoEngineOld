#pragma once

#include "Struct.h"
#include "Class.gen.h"

namespace ZE::Refl
{

// A C++ class
ZCLASS()
class REFLECTION_API CClass : public CStruct
{
	ZE_REFL_BODY()

public:
	CClass(const char* InName,
		const uint64_t& InSize)
		: CStruct(InName, InSize), bIsInterface(false) {}

	void SetIsInterface(const bool& bInIsInterface) { bIsInterface = bInIsInterface; bIsInstanciable = !bIsInterface; }

	bool IsInterface() const { return bIsInterface; }
private:
	bool bIsInterface;
};

REFLECTION_API void RegisterClass(const CClass* InStruct);
REFLECTION_API const CClass* GetClassByName(const std::string& InName);
REFLECTION_API const std::vector<const CClass*> GetClasses();

template<typename T>
ZE_FORCEINLINE const CClass* GetClass()
{
    return GetClassByName(TTypeName<T>::Name);
}

}