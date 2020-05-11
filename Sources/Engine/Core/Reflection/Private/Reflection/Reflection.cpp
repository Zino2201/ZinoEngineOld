#include "Reflection/Reflection.h"
#include "Module/Module.h"
#include "Reflection/Struct.h"
#include "Reflection/Class.h"
#include "Reflection/Builders.h"

DEFINE_MODULE(ZE::CDefaultModule, "Reflection");

namespace ZE::Refl
{

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

std::vector<CClass*> GetDerivedClassesFrom(CClass* InParent)
{
	std::vector<CClass*> Classes;
	Classes.reserve(10);

	for (const auto& Class : CClass::GetClasses())
	{
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
		Builders::TTypeBuilder<int>("int");
		Builders::TTypeBuilder<long>("long");
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
		//Builders::TTypeBuilder<int32_t>("int32_t");
		Builders::TTypeBuilder<int64_t>("int64_t");

		/** Reflection classes */
		Builders::TClassBuilder<CType>("CType")
			.Ctor<const char*,
			const uint64_t&>();

		Builders::TClassBuilder<CStruct>("CStruct")
			.Ctor<const char*,
			const uint64_t&>();

		Builders::TClassBuilder<CClass>("CClass")
			.Ctor<const char*,
			const uint64_t&>();
	}
}

}
