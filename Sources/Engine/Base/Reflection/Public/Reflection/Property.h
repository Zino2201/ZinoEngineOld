#pragma once

#include "EngineCore.h"
#include "Type.h"
#include "Property.gen.h"

namespace ZE::Refl
{

/**
 * Property flags
 */
enum class EPropertyFlagBits
{
	None = 0,

	/** Allow the property to be serialized */
	Serializable = 1 << 0,
};
ENABLE_FLAG_ENUMS(EPropertyFlagBits, EPropertyFlags);

/**
 * A property
 */
ZCLASS()
class REFLECTION_API CProperty
{
    ZE_REFL_BODY()

public:
	CProperty(const char* InName,
		const uint64_t& InSize,
		const size_t& InOffset,
		const EPropertyFlags& InFlags) :
		Name(InName), Size(InSize), Offset(InOffset), Flags(InFlags) {}

	void* GetData(void* InPtr) const
	{
		uint8_t* Dst = reinterpret_cast<uint8_t*>(InPtr);
		return Dst + Offset;
	}

	template<typename T>
	T* GetData(void* InPtr) const
	{
		return reinterpret_cast<T*>(GetData(InPtr));
	}
private:
    const char* Name;
    size_t Size;
	size_t Offset;
	EPropertyFlags Flags; 
};

}