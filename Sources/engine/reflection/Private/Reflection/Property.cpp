#include "reflection/Property.h"
#include "reflection/detail/PropertyImpl.h"

namespace ze::reflection
{

Property::Property(const std::string& in_name,
		const std::string& in_type_name,
		const size_t& in_offset,
		const robin_hood::unordered_map<std::string, std::string>& in_metadatas) : name(in_name), type(in_type_name),
		offset(in_offset), metadata(in_metadatas) {}

Property::Property(Property&& other) : 
	name(std::move(other.name)),
	type(std::move(other.type)),
	offset(std::move(other.offset)),
	flags(std::move(other.flags)),
	impl(std::move(other.impl)),
	metadata(std::move(other.metadata)) {}


void Property::operator=(Property&& other) 
{
	name = std::move(other.name);
	type = std::move(other.type);
	offset = std::move(other.offset);
	flags = std::move(other.flags);
	impl = std::move(other.impl);
	metadata = std::move(other.metadata);
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