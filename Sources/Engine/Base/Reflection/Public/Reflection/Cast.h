#pragma once

#include "Class.h"
#include "Traits.h"

namespace ze::reflection
{

/**
 * Safe cast that performs runtime check to ensure that the pointer can actually
 *	be casted to the target type
 * Returns nullptr if the pointer cannot be casted
 */
template<typename To, typename From>
	requires IsReflClass<To> && IsReflClass<From>
To* cast(From* from)
{
	const Class* from_class = from->get_class();
	const Class* to_class = Class::get<To>();

	if(from_class->is_derived_from(to_class))
		return static_cast<To*>(from);

	return nullptr;
}

}