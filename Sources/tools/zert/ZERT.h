#pragma once

#include <string_view>
#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <robin_hood.h>

/**
 * From boost's hash_combine
 */
template <class T, class H = std::hash<T>>
inline void hash_combine(std::size_t& Seed, const T& V)
{
	H Hasher;
	Seed ^= Hasher(V) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
}

/** Flags */
#include <type_traits>

#define DECLARE_FLAG_ENUM(EnumType) \
	inline EnumType operator~ (EnumType a) { return (EnumType)~(std::underlying_type<EnumType>::type)a; } \
	inline EnumType operator| (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a | (std::underlying_type<EnumType>::type)b); } \
	inline EnumType operator& (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a & (std::underlying_type<EnumType>::type)b); } \
	inline EnumType operator^ (EnumType a, EnumType b) { return (EnumType)((std::underlying_type<EnumType>::type)a ^ (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator|= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a |= (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator&= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a &= (std::underlying_type<EnumType>::type)b); } \
	inline EnumType& operator^= (EnumType& a, EnumType b) { return (EnumType&)((std::underlying_type<EnumType>::type&)a ^= (std::underlying_type<EnumType>::type)b); }
#define HAS_FLAG(Enum, Other) (Enum & Other) == Other
#define HASN_FLAG(Enum, Other) !(HAS_FLAG(Enum, Other))

void fatal(std::string_view message, ...);
std::string read_text_file(const std::string_view& in_filename);
std::vector<std::string> read_file_lines(const std::string_view& in_filename);
std::vector<std::string> tokenize(const std::string& in_string,
	const char& in_delimiter = ' ');

/** Constants */
static constexpr std::array<const char*, 11> primitive_types =
{
	"bool",
	"uint8_t",
	"uint16_t",
	"uint32_t",
	"uint64_t",
	"int8_t",
	"int16_t",
	"int32_t",
	"int64_t",
	"float",
	"double"
};

static const robin_hood::unordered_map<std::string_view, std::string_view> primitive_types_stdint_map =
{
	{ "unsigned char", "uint8_t" },
	{ "unsigned int", "uint32_t" },
	{ "unsigned short", "uint16_t" },
	{ "unsigned int", "uint32_t" },
	{ "unsigned long", "uint32_t/uint64_t" },
	{ "unsigned long long", "uint64_t" },
	{ "short", "int16_t" },
	{ "int", "int32_t" },
	{ "long", "int32_t/int64_t" },
	{ "long long", "int64_t" },
}; 