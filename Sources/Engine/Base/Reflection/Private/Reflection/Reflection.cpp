#include "Reflection/Reflection.h"
#include "Module/Module.h"
#include "Reflection/Struct.h"
#include "Reflection/Class.h"
#include "Reflection/Builders.h"
#include "Reflection/Serialization.h"

DEFINE_MODULE(ZE::Module::CDefaultModule, Reflection);

namespace ZE::Refl
{

std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(void*, void*)>>> ArchiveMap;


std::unordered_map<std::string, std::function<void(void*, void*)>>& GetArchiveMap(const char* InArchive)
{
	return ArchiveMap[InArchive];
}

std::vector<CStruct*> GetDerivedStructsFrom(CStruct* InParent)
{
	std::vector<CStruct*> Structs;
	Structs.reserve(10);

	for(const auto& Struct : CStruct::GetStructs())
	{
		if(Struct->IsDerivedFrom(InParent))
			Structs.push_back(Struct);
	}

	return Structs;
}

std::vector<CClass*> GetDerivedClassesFrom(CClass* InParent, const bool& bInIncludeParentClass)
{
	std::vector<CClass*> Classes;
	Classes.reserve(10);

	for (const auto& Class : CClass::GetClasses())
	{
		if (Class == InParent && !bInIncludeParentClass)
			continue;

		if (Class->IsDerivedFrom(InParent))
			Classes.push_back(Class);
	}

	return Classes;
}

REFL_INIT_BUILDERS_FUNC(Reflection)
{
	{
		/** Register basic types */
		Builders::TTypeBuilder<bool>("bool");
		Builders::TTypeBuilder<float>("float");
		Builders::TTypeBuilder<double>("double");

		/** Unsigned types */
		Builders::TTypeBuilder<uint8_t>("uint8_t");
		Builders::TTypeBuilder<uint16_t>("uint16_t");
		Builders::TTypeBuilder<uint32_t>("uint32_t");
		Builders::TTypeBuilder<uint64_t>("uint64_t");

		/** Signed types */
		Builders::TTypeBuilder<int8_t>("int8_t");
		Builders::TTypeBuilder<int16_t>("int16_t");
		Builders::TTypeBuilder<int32_t>("int32_t");
		Builders::TTypeBuilder<int64_t>("int64_t");
	}
}

}
