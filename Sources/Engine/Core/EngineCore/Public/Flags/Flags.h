#pragma once

#include <type_traits>

namespace ZE
{

template<typename T>
	requires std::is_enum_v<T>
struct TFlags
{
public:
	using MaskType = typename std::underlying_type<T>::type;

    constexpr TFlags() : Mask(0) {}
    constexpr TFlags(const T& InBit) : Mask(static_cast<MaskType>(InBit)) {}
    constexpr explicit TFlags(const MaskType& InMask) : Mask(InMask) {}

	/** Bitwise operators */
    friend constexpr TFlags<T> operator&(const TFlags<T>& InLeft, const TFlags<T>& InRight) 
    {
        return TFlags<T>(InLeft.Mask & InRight.Mask);
    }

	friend constexpr TFlags<T> operator|(const TFlags<T>& InLeft, const TFlags<T>& InRight)
	{
		return TFlags<T>(InLeft.Mask| InRight.Mask);
	}

	friend constexpr TFlags<T> operator^(const TFlags<T>& InLeft, const TFlags<T>& InRight)
	{
		return TFlags<T>(InLeft.Mask ^ InRight.Mask);
	}

	constexpr TFlags<T> operator~() const
	{
		return TFlags<T>(~Mask);
	}

	/** Assignements operators */
	constexpr TFlags<T> operator=(const TFlags<T>& InOther)
	{
		Mask = InOther.Mask;
		return *this;
	}

	constexpr TFlags<T> operator|=(const TFlags<T> & InOther) const
	{
		Mask |= InOther.Mask;
		return *this;
	}

	constexpr TFlags<T> operator&=(const TFlags<T> & InOther) const
	{
		Mask &= InOther.Mask;
		return *this;
	}

	constexpr TFlags<T> operator^=(const TFlags<T> & InOther) const
	{
		Mask ^= InOther.Mask;
		return *this;
	}

	explicit constexpr operator MaskType() const
	{
		return Mask;
	}

	constexpr explicit operator bool() const { return Mask != 0; }
private:
	MaskType Mask;
};

#define ENABLE_FLAG_ENUMS(EnumType, FlagsType) \
	constexpr auto operator&(EnumType InLeft, EnumType InRight) \
	{ \
		return TFlags<EnumType>(InLeft) & InRight; \
	} \
	constexpr auto operator|(EnumType InLeft, EnumType InRight) \
	{ \
		return TFlags<EnumType>(InLeft) | InRight; \
	} \
	constexpr auto operator^(EnumType InLeft, EnumType InRight) \
	{ \
		return TFlags<EnumType>(InLeft) ^ InRight; \
	} \
	using FlagsType = TFlags<EnumType>;

}