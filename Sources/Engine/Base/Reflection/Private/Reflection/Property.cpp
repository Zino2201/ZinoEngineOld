#include "Reflection/Property.h"
#include "Reflection/Detail/PropertyImpl.h"

namespace ze::reflection
{

Property::Property(const std::string& in_name,
		const std::string& in_type_name,
		const size_t& in_offset,
		const PropertyFlags& in_flags) : name(in_name), type(in_type_name),
		offset(in_offset), flags(in_flags) {}

Property::Property(Property&& other) : 
	name(std::move(other.name)),
	type(std::move(other.type)),
	offset(std::move(other.offset)),
	flags(std::move(other.flags)),
	impl(std::move(other.impl)) {}


void Property::operator=(Property&& other) 
{
	name = std::move(other.name);
	type = std::move(other.type);
	offset = std::move(other.offset);
	flags = std::move(other.flags);
	impl = std::move(other.impl);
}

Property::~Property() = default;

Any Property::get_value(const void* instance) const
{
	return impl->get_value(instance);
}

void* Property::get_value_ptr(const void* instance) const
{
	return impl->get_value_ptr(instance);
}

void Property::set_value(const void* instance, const std::any& value) const
{
	impl->set_value(instance, value);
}

}