#pragma once

#include "ZRT.h"
#include <robin_hood.h>

/**
 * Flags about this property
 */
enum class EPropertyAttributes
{
	None = 0,

	Constexpr = 1 << 0,
	Const = 1 << 1,
	Mutable = 1 << 2,
	Volatile = 1 << 3,
};
DECLARE_FLAG_ENUM(EPropertyAttributes);

/**
 * Flags about this property
 */
enum class EPropertyFlags
{
	None = 0,

	Serializable = 1 << 0,
	Visible = 1 << 1,
	Editable = 1 << 2,
};
DECLARE_FLAG_ENUM(EPropertyFlags);

enum class EAccess
{
	Public,
	Protected,
	Private
};

/**
 * A property
 */
struct SProperty
{
	std::string Name;
	std::string Type;
	EAccess Access;
	EPropertyFlags Flags;
	EPropertyAttributes Attributes;

	SProperty(const std::string& InName, const std::string& InType,
		const EAccess& InAccess, const EPropertyFlags& InFlags, const EPropertyAttributes& InAttribs) : Name(InName),
		Type(InType), Access(InAccess), Flags(InFlags), Attributes(InAttribs) {}
};

static const robin_hood::unordered_map<std::string, EPropertyFlags> GPropFlagMap = 
{
	{ "Serializable", EPropertyFlags::Serializable },
	{ "Visible", EPropertyFlags::Visible },
	{ "Editable", EPropertyFlags::Editable },
};

enum class EStructFlags
{
	None = 1 << 0,

	HideInEditor = 1 << 1,
};
DECLARE_FLAG_ENUM(EStructFlags);

static const robin_hood::unordered_map<std::string, EStructFlags> GStructClassMap = 
{
	{ "HideInEditor", EStructFlags::HideInEditor },
};

/**
 * Type "database"
 */
class CTypeDatabase
{
public:
	static CTypeDatabase& Get()
	{
		static CTypeDatabase Instance;
		return Instance;
	}

	void RegisterType(const std::string& InType)
	{
		Types.insert(InType);
	}

	bool HasType(const std::string& InType) const
	{
		if(InType.find("*") != std::string::npos ||
			InType.find("&") != std::string::npos)
			return true;
			
		for(const auto& Templated : GAcceptedTemplatedTypes)
		{
			if(InType.find(Templated) != std::string::npos)
				return true;
		}

		return Types.count(InType);
	}

	const auto& GetTypes() const { return Types; }
public:
	CTypeDatabase(const CTypeDatabase&) = delete;
	void operator=(const CTypeDatabase&) = delete;
private:
	CTypeDatabase() = default;
private:
	robin_hood::unordered_set<std::string> Types;
};

/**
 * A C++ struct
 */
class CStruct
{
public:
	CStruct(const std::string& InName, const std::string& InNamespace,
		const size_t& InDeclLine = 0) : Name(InName), 
		Namespace(InNamespace), BodyLine(-1), DeclLine(InDeclLine) {}

	void AddProperty(const std::string& InName, const std::string& InType,
		const EAccess& InAccess, 
		const EPropertyFlags& InFlags, const EPropertyAttributes& InAttribs);

	void AddParent(const std::string& InParent);
	void SetBodyLine(const size_t& InBodyLine) { BodyLine = InBodyLine; }
	void AddCtor(const std::string& InStr) { Ctors.emplace_back(InStr); }

	const std::vector<SProperty>& GetProperties() const { return Properties; }
	const std::vector<std::string>& GetParents() const { return Parents; }
	const std::string& GetName() const { return Name; }
	const std::string& GetNamespace() const { return Namespace; }
	const std::vector<std::string>& GetCtors() const { return Ctors; }
	const size_t& GetBodyLine() const { return BodyLine; }
	const size_t& GetDeclLine() const { return DeclLine; }
	EStructFlags& GetFlags() { return flags; }
	const EStructFlags& GetFlags() const { return flags; }
private:
	std::string Name;
	std::vector<SProperty> Properties;
	std::vector<std::string> Parents;
	std::string Namespace;
	std::vector<std::string> Ctors;
	EStructFlags flags = EStructFlags::None;
	size_t BodyLine;
	size_t DeclLine;
};