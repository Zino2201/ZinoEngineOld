#pragma once

#include "Macros.h"
#include "Serialization/Archive.h"
#include "Serialization/Traits.h"
#include "Singleton.h"
#include <robin_hood.h>

/**
 * Serialization using reflection
 * Largely inspired by cereal and boost polymorphic archives
 */

namespace ZE::Refl
{

template<typename T>
static constexpr const char* TArchiveName = "";

/**
 * Get or create an map for the specified archive name
 */
REFLECTION_API std::unordered_map<std::string, std::function<void(void*, void*)>>& GetArchiveMap(const char* InArchive);

/**
 * Create an binding for a Archive, T when instantiated
 */
template<typename T, typename Archive>
struct TArchiveBindingCreator 
{
	TArchiveBindingCreator()
	{
		auto& Map = GetArchiveMap(TArchiveName<Archive>);

		Map.insert({ TTypeName<T>::Name, [] (void* InArchive, void* InObj)
		{
			ZE::Serialization::Serialize(
				reinterpret_cast<Archive&>(*InArchive),
				reinterpret_cast<T&>(*InObj));
		}});
	}
};

/** Registration */
template<typename T, typename Archive>
struct TArchiveCreateBindings
{
	static const TArchiveBindingCreator<T, Archive>& CreateBindings()
	{
		return TSingleton<TArchiveBindingCreator<T, Archive>>::Get();
	}
};

/**
 * This will register the archive
 * This works by being instantiated using a ZE__Refl_RegisterArchive function
 * that returns this type
 */
template<typename T, typename Archive>
struct TRegisterArchive
{
	template<void(*)()>
	struct TInstantiateFunction {};

	/**
	 * Instantiated per type
	 */
	REFLECTION_API static void Instantiate()
	{
		TArchiveCreateBindings<T, Archive>::CreateBindings();
	}

	/** Instantiate the function */
	using InstantiateFunctionType = TInstantiateFunction<&TRegisterArchive::Instantiate>;
};

struct TAdlHelper {};

/**
 * Register an archive for serialization using reflection 
 */
#define ZE_REFL_REGISTER_ARCHIVE(Archive) \
	namespace ZE::Refl \
	{  \
		template<typename T> typename ZE::Refl::TRegisterArchive<T, Archive>::type ZE__Refl_RegisterArchive(T*, Archive*, TAdlHelper);  \
		template<> \
		static constexpr const char* TArchiveName<Archive> = #Archive; \
	}

template<typename T> 
struct TRegisterTypeToArchive 
{
	TRegisterTypeToArchive()
	{ 
		static_assert(ZE::Refl::TIsReflStruct<T> || ZE::Refl::TIsReflClass<T>,
			"ZE_REFL_SERL_REGISTER_TYPE only works with reflected types");
		ZE::Refl::ZE__Refl_RegisterArchive(static_cast<T*>(nullptr), 0, TAdlHelper{});
	}
};

template<typename T>
static constexpr bool TIsSerializableWithReflection = false;

/**
 * Declare a reflected serializable type
 */
#define ZE_REFL_SERL_DECLARE_TYPE(Type) \ 
	namespace ZE::Refl \
	{ \
		template<> \
		static constexpr bool TIsSerializableWithReflection<Type> = true; \
	}

/**
 * Register an type
 * The macro must be called in global namespace
 */
#define ZE_REFL_SERL_REGISTER_TYPE(Type, UniqueName) \ 
	namespace ZE::Refl \
	{ \
		static const TRegisterTypeToArchive<Type>& ZE_CONCAT(ZE__Refl_Serl_Inst_Ref_, UniqueName) = \ 
			TSingleton<TRegisterTypeToArchive<Type>>::Get(); \
	}

/**
 * Default ZE__Refl_RegisterArchive function
 */
template<typename T>
void ZE__Refl_RegisterArchive(T*, int, TAdlHelper) {}
}

template<typename ArchiveType, typename T>
ZE_FORCEINLINE void Serialize(ArchiveType& InArchive, const std::unique_ptr<T>& InPtr)
	requires ZE::Refl::TIsSerializableWithReflection<T>
{
	if(!InPtr)
		InPtr = std::make_unique<T>();

	SerializeRefl(InArchive, *InPtr.get());
}

/**
 * Serialize an reflection type
 */
template<typename ArchiveType, typename T>
void SerializeRefl(ArchiveType& InArchive, T& InObj)
	requires ZE::Refl::TIsSerializableWithReflection<T>
{
	auto& Map = ZE::Refl::GetArchiveMap(ZE::Refl::TArchiveName<ArchiveType>);

	std::string Name;
	if constexpr (ZE::Refl::TIsReflStruct<T>)
		Name = InObj.GetStruct()->GetName();

	if constexpr (ZE::Refl::TIsReflClass<T>)
		Name = InObj.GetClass()->GetName();

	auto Serializer = Map.find(Name);

	verify(Serializer != Map.end());

	if(Serializer != Map.end())
	{
		Serializer->second(reinterpret_cast<void*>(&InArchive),
			reinterpret_cast<void*>(&InObj));
	}
}

/**
 * Serialize all property of the specified reflected type
 */
inline void SerializeProp()
{

}