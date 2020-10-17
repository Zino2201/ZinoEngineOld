#include "Reflection/Registration.h"
#include "Reflection/Builders.h"
#include "Module/Module.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, Reflection);

namespace ze::reflection
{

ZE_REFL_BUILDER_FUNC(Reflection_Primitives)
{
	builders::TypeBuilder<bool>("bool");
	builders::TypeBuilder<float>("float");
	builders::TypeBuilder<double>("double");

	builders::TypeBuilder<uint8_t>("uint8_t");
	builders::TypeBuilder<uint16_t>("uint16_t");
	builders::TypeBuilder<uint32_t>("uint32_t");
	builders::TypeBuilder<uint64_t>("uint64_t");

	builders::TypeBuilder<int8_t>("int8_t");
	builders::TypeBuilder<int16_t>("int16_t");
	builders::TypeBuilder<int32_t>("int32_t");
	builders::TypeBuilder<int64_t>("int64_t");
}

}