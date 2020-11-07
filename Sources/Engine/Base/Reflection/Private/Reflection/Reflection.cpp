#include "Reflection/Registration.h"
#include "Reflection/Builders.h"
#include "Module/Module.h"
#include "Maths/Vector.h"

ZE_DEFINE_MODULE(ze::module::DefaultModule, Reflection);

namespace ze::reflection
{

ZE_REFL_BUILDER_FUNC(Reflection_Primitives)
{
	builders::TypeBuilder<bool>();
	builders::TypeBuilder<float>();
	builders::TypeBuilder<double>();

	builders::TypeBuilder<uint8_t>();
	builders::TypeBuilder<uint16_t>();
	builders::TypeBuilder<uint32_t>();
	builders::TypeBuilder<uint64_t>();

	builders::TypeBuilder<int8_t>();
	builders::TypeBuilder<int16_t>();
	builders::TypeBuilder<int32_t>();
	builders::TypeBuilder<int64_t>();

	builders::ClassBuilder<ze::maths::Vector3f>();
	builders::ClassBuilder<ze::maths::Vector3d>();
}

}