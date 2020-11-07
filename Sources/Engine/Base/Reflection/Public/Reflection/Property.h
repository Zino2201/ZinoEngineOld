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
	None = 0,

	Serializable = 1 << 0,
	Visible = 1 << 1,
	Editable = 1 << 2,
};
ENABLE_FLAG_ENUMS(PropertyFlagBits, PropertyFlags)

class REFLECTION_API Property
{
public:
	Property(const std::string& in_name,
		const std::string& in_type_name,
		const size_t& in_offset,
		const PropertyFlags& in_flags);
	~Property();

	Property(Property&& other);
	Property(const Property&) = delete;
	void operator=(const Property&) = delete;

	Any get_value(const void* instance) const;

	/**
	 * Get a pointer to the contained value
	 * \param instance
	 */
	void* get_value_ptr(const void* instance) const;
	void set_value(const void* instance, const std::any& value) const;

	ZE_FORCEINLINE const std::string& get_name() const { return name; }
	ZE_FORCEINLINE const Type* get_type() const { return type.get(); }
	ZE_FORCEINLINE const size_t& get_offset() const { return offset; }
	ZE_FORCEINLINE const PropertyFlags& get_flags() const { return flags; }
	ZE_FORCEINLINE const std::unique_ptr<detail::PropertyImplBase>& get_impl() const { return impl; }
private:
	std::string name;
	LazyTypePtr type;
	size_t offset;
	PropertyFlags flags;
	std::unique_ptr<detail::PropertyImplBase> impl;
};

}