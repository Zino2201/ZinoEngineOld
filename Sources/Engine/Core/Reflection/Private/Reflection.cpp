#include "Reflection.h"
#include "Module/Module.h"

DEFINE_MODULE(ZE::CDefaultModule, "Reflection");

namespace ZE::Refl
{

std::vector<std::unique_ptr<CType>> CType::Types;

void CType::RegisterType(const char* InName,
	const uint64_t& InSize)
{
	Types.push_back(std::make_unique<CType>(InName, InSize));
}

void CType::RegisterStruct(const char* InName,
	const uint64_t& InSize,
	const STypeStruct& InStruct)
{
	Types.push_back(std::make_unique<CType>(InName, InSize, InStruct));
}

struct SMyStruct
{
	DECLARE_REFL_STRUCT_OR_CLASS()

	int Test;
	int Test2;
	int Test3;
};

REFL_INIT_BUILDERS_FUNC()
{
	{
		/** Register basic types */
		Builders::TTypeBuilder<int>("int");

		Builders::TStructBuilder<SMyStruct>("SMyStruct")
			.Property("Test", &SMyStruct::Test)
			.Property("Test2", &SMyStruct::Test2)
			.Property("Test3", &SMyStruct::Test3);

	}

	auto& Types = CType::Types;
	CType* S = CType::Get<SMyStruct>();
	//CType* I = CType::Get<int>();

	__debugbreak();
}

}