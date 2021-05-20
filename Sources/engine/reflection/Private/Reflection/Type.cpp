#include "reflection/Type.h"
#include "reflection/Registration.h"

namespace ze::reflection
{

const Type* Type::get_by_name(const std::string& in_name)
{
	const auto& reg_mgrs = get_registration_managers();

	for(const auto& reg_mgr : reg_mgrs)
	{
		if(const Type* type = reg_mgr->get_type(in_name))
			return type;
	}

	return nullptr;
}

}