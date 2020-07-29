#pragma once

#include "Struct.h"
#include "Class.gen.h"

namespace ZE::Refl
{

// A C++ class
ZCLASS()
class REFLECTION_API CClass : public CStruct
{
	REFL_BODY()

public:
	CClass(const char* InName,
		const uint64_t& InSize)
		: CStruct(InName, InSize), bIsInterface(false) {}

	static void AddClass(CClass* InClass);

	template<typename T>
	static CClass* Get()
	{
		auto Class = Get(TTypeName<T>::Name);
		must(Class); // This class is not registered

		return Class;
	}

	static CClass* Get(const char* InName);

	void SetIsInterface(const bool& bInIsInterface) { bIsInterface = bInIsInterface; bIsInstanciable = !bIsInterface; }

	bool IsInterface() const { return bIsInterface; }

	static const std::vector<CClass*>& GetClasses() { return Classes; }
private:
	bool bIsInterface;

	inline static std::vector<CClass*> Classes;
};

}