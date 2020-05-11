#pragma once

#include "Struct.h"

namespace ZE::Refl
{

// A C++ class
class CClass : public CStruct
{
	DECLARE_REFL_STRUCT_OR_CLASS1(CClass, CStruct)

public:
	CClass(const char* InName,
		const uint64_t& InSize)
		: CStruct(InName, InSize), bIsInterface(false) {}

	REFLECTION_API static void AddClass(CClass* InClass);

	template<typename T>
	static CClass* Get()
	{
		auto Class = Get(TTypeName<T>::Name);
		must(Class); // This class is not registered

		return Class;
	}

	REFLECTION_API static CClass* Get(const char* InName);

	void SetIsInterface(const bool& bInIsInterface) { bIsInterface = bInIsInterface; bIsInstanciable = !bIsInterface; }

	bool IsInterface() const { return bIsInterface; }

	REFLECTION_API static const std::vector<CClass*>& GetClasses() { return Classes; }
private:
	bool bIsInterface;

	REFLECTION_API inline static std::vector<CClass*> Classes;
};
DECLARE_REFL_TYPE(CClass);

}