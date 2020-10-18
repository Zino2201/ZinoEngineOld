#pragma once

#include <type_traits>
#include <functional>

namespace ze
{

template<typename T>
	requires std::is_enum_v<T>
struct Flags
{
public:
	using MaskType = typename std::underlying_type<T>::type;

    constexpr Flags() : mask (0) {}
    constexpr Flags(const T& bit) : mask(static_cast<MaskType>(bit)) {}
    constexpr explicit Flags(const MaskType& in_mask) : mask(in_mask) {}

	/** Bitwise operators */
    friend constexpr Flags<T> operator&(const Flags<T>& left, const Flags<T>& right) 
    {
        return Flags<T>(left.mask & right.mask);
    }

	friend constexpr Flags<T> operator|(const Flags<T>& left, const Flags<T>& right)
	{
		return Flags<T>(left.mask | right.mask);
	}

	friend constexpr Flags<T> operator^(const Flags<T>& left, const Flags<T>& right)
	{
		return Flags<T>(left.mask ^ right.mask);
	}

	constexpr Flags<T> operator~() const
	{
		return Flags<T>(~mask);
	}
	
	/** Compare operators */
	friend constexpr bool operator==(const Flags<T>& left, const Flags<T>& right)
	{
		return left.mask == right.mask;
	}

	friend constexpr bool operator!=(const Flags<T>& left, const Flags<T>& right)
	{
		return left.mask != right.mask;
	}

	/** Assignements operators */
	constexpr Flags<T> operator=(const Flags<T>& other)
	{
		mask = other.mask;
		return *this;
	}

	constexpr Flags<T> operator|=(const Flags<T> & other)
	{
		mask |= other.mask;
		return *this;
	}

	constexpr Flags<T> operator&=(const Flags<T> & other)
	{
		mask &= other.mask;
		return *this;
	}

	constexpr Flags<T> operator^=(const Flags<T> & other)
	{
		mask ^= other.mask;
		return *this;
	}

	explicit constexpr operator MaskType() const
	{
		return mask;
	}

	constexpr explicit operator bool() const { return mask != 0; }
private:
	MaskType mask;
};

#define ENABLE_FLAG_ENUMS(EnumType, FlagsType) \
	constexpr auto operator&(EnumType left, EnumType right) \
	{ \
		return ze::Flags<EnumType>(left) & right; \
	} \
	constexpr auto operator|(EnumType left, EnumType right) \
	{ \
		return ze::Flags<EnumType>(left) | right; \
	} \
	constexpr auto operator^(EnumType left, EnumType right) \
	{ \
		return ze::Flags<EnumType>(left) ^ right; \
	} \
	using FlagsType = ze::Flags<EnumType>;

}

namespace std
{
	template<typename T> 
	struct hash<ze::Flags<T>>
	{
		ZE_FORCEINLINE uint64_t operator()(const ze::Flags<T>& in_flags) const
		{
			return std::hash<ze::Flags<T>::MaskType>()(static_cast<ze::Flags<T>::MaskType>(in_flags));
		}
	};
}