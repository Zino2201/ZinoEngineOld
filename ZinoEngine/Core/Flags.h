#pragma once

#define DECLARE_FLAG_ENUM(EnumType) \
	inline EnumType operator~ (EnumType a) { return (EnumType)~(std::underlying_type<EnumType>::type)a; } \
	inline EnumType operator| (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a | (std::underlying_type<EnumType>::type)b); } \
	inline EnumType operator& (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a & (std::underlying_type<EnumType>::type)b); } \
	inline EnumType operator^ (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a ^ (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator|= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a |= (std::underlying_type<EnumType>::type)b); }; \
	inline EnumType& operator&= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a &= (std::underlying_type<EnumType>::type)b); }; \
	inline EnumType& operator^= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a ^= (std::underlying_type<EnumType>::type)b); }; \
	using EnumType##Flags = EnumType;
#define HAS_FLAG(Enum, Other) (Enum & Other) == Other