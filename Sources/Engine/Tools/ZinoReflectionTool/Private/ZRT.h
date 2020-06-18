#pragma once

#include <string_view>
#include <vector>
#include <string>
#include <array>
#include <filesystem>
#include <map>

/**
 * From boost's hash_combine
 */
template <class T, class H = std::hash<T>>
inline void HashCombine(std::size_t& Seed, const T& V)
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

void Fatal(std::string_view InMessage, ...);
std::vector<std::string> ReadTextFile(const std::string_view& InFilename);
std::vector<std::string> Tokenize(const std::string& InString,
	const char& InDelimiter = ' ');

/**
 * Types
 */
static constexpr std::array<std::string_view, 10> GAcceptedTypes =
{
	"bool",
	"long",
	"uint8_t",
	"uint16_t",
	"uint32_t",
	"uint64_t",
	"int8_t",
	"int16_t",
	"int32_t",
	"int64_t"
};

static const std::map<std::string_view, std::string_view> GKnownUnsupportedTypes =
{
	{ "unsigned char", "uint8_t" },
	{ "unsigned int", "uint32_t" },
	{ "unsigned short", "uint16_t" },
	{ "short", "int16_t" },
	{ "int", "int32_t" },
}; 

static constexpr std::array<std::string_view, 1> GAcceptedTemplatedTypes =
{
	"std::vector"
};

/**
 * Macros
 */
static constexpr std::string_view GEnumMacro = "ZENUM";
static constexpr std::string_view GStructMacro = "ZSTRUCT";
static constexpr std::string_view GClassMacro = "ZCLASS";
static constexpr std::string_view GPropertyMacro = "ZPROPERTY";
static constexpr std::string_view GFunctionMacro = "ZFUNCTION";
static constexpr std::string_view GApiMacro = "_API";
static constexpr std::string_view GReflBuilderMacro = "REFL_INIT_BUILDERS_FUNC";
static constexpr std::string_view GReflBodyMacro = "REFL_BODY";

/**
 * C++ related
 */

// Accessors
static constexpr std::string_view GPublicAccessor = "public";
static constexpr std::string_view GProtectedAccessor = "protected";
static constexpr std::string_view GPrivateAccessor = "private";
// Specifiers
static constexpr std::string_view GConstExpr = "constexpr";
static constexpr std::string_view GConst = "const";
static constexpr std::string_view GMutable = "mutable";
static constexpr std::string_view GVolatile = "volatile";
// Misc
static constexpr std::string_view GStructClassEnumEnding = "};";

/**
 * Writer related
 */
static constexpr std::string_view GGeneratedFileHeader = "#pragma once\n\n/**\n * GENERATED FILE BY ZINOREFLECTIONTOOL. DO NOT MODIFY !\n */";
static constexpr std::string_view GGeneratedFileCpp = "/**\n * GENERATED FILE BY ZINOREFLECTIONTOOL. DO NOT MODIFY !\n */";
static constexpr std::string_view GGeneratedHeaderIncludes = "#include \"Reflection/Macros.h\"\n\n";

/** .gen.h **/
static constexpr std::string_view GDeclareReflTypeMacro = "REFL_SPECIALIZE_TYPE_NAME(";
static constexpr std::string_view GReflBodyDefMacro = "_Refl_Body_";
static constexpr std::string_view GDeclareStructMacro = "DECLARE_STRUCT(";
static constexpr std::string_view GDeclareClassMacro = "DECLARE_CLASS(";

/** gen.cpp */
static constexpr std::string_view GStructBuilder = "Builders::TStructBuilder";
static constexpr std::string_view GClassBuilder = "Builders::TClassBuilder";