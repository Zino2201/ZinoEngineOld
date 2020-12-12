#pragma once

#include "EngineCore.h"
#include "Flags/Flags.h"
#include "Type.h"
#include "Any.h"
#include <string>
#include <cstdint>

namespace ze::reflection
{

namespace detail { class PropertyImplBase; }

enum class PropertyFlagBits
{
	
};

ENABLE_FLAG_ENUMS(PropertyFlagBits, PropertyFlags)

class REFLECTION_API Property
{
public:
	Property(const std::string& in_name,
		const std::string& in_type_name,
		const size_t& in_offset,
		const robin_hood::unordered_map<std::string, std::string>& in_metadatas);
	~Property();

	Property(Property&& other);
	void operator=(Property&& other);

	Property(const Property&) = delete;
	void operator=(const Property&) = delete;

	Any get_value(const void* instance) const;

	/**
	 * Get a pointer to the contained value
	 * \param instance
	 */
	void* get_value_ptr(const void* instance) const;
	void set_value(const void* instance, const std::any& value) const;
	
	template<typename ArchiveType>
	void serialize_value(ArchiveType& in_archive, void* instance) const
	{
		reflection::Any value = get_value(instance);
		value.serialize(in_archive);
		if constexpr(ze::serialization::IsInputArchive<ArchiveType>)
		{
			memmove(get_value_ptr(instance), 
				value.get_value_ptr(), type.get()->get_size());
		}
	}

	ZE_FORCEINLINE const std::string& get_name() const { return name; }
	ZE_FORCEINLINE const Type* get_type() const { return type.get(); }
	ZE_FORCEINLINE const size_t& get_offset() const { return offset; }
	ZE_FORCEINLINE const PropertyFlags& get_flags() const { return flags; }
	ZE_FORCEINLINE const std::unique_ptr<detail::PropertyImplBase>& get_impl() const { return impl; }
	ZE_FORCEINLINE bool has_metadata(const std::string& in_key) const { return metadata.contains(in_key); }

	ZE_FORCEINLINE bool operator<(const Property& other) const { return name < other.name; }
	ZE_FORCEINLINE bool operator>(const Property& other) const { return name > other.name; }
private:
	std::string name;
	LazyTypePtr type;
	size_t offset;
	PropertyFlags flags;
	std::unique_ptr<detail::PropertyImplBase> impl;
	robin_hood::unordered_map<std::string, std::string> metadata;
};

}