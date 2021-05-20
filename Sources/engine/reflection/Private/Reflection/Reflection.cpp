#include "reflection/Registration.h"
#include "reflection/Builders.h"
#include "module/Module.h"
#include "maths/Vector.h"
#include "reflection/Serialization.h"
#include "reflection/VectorRefl.h"

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

namespace serialization
{

robin_hood::unordered_map<std::string, robin_hood::unordered_map<std::string, std::function<void(void*, void*)>>> archive_map;

robin_hood::unordered_map<std::string, std::function<void(void*, void*)>>& get_archive_map(const char* in_archive)
{
	return archive_map[in_archive];
}

void free_archive_map()
{
	archive_map.clear();
}

}

}