#pragma once

#include "ZRT.h"
#include "Header.h"

/**
 * Parser to generate a CHeader from a .h
 */
class CParser
{
	enum class EType
	{
		None,
		Struct,
		Class,
	};
public:
	CParser(CHeader* InHeader, const std::string_view& InPathHeader,
		const bool& bInIgnorePropertiesAndFunctions = false);
private:
	std::string GetObjectName(const std::vector<std::string>& InLines,
		const std::vector<std::string>& InWords, const size_t& InLine, size_t& InNameLine,
		size_t& InWordIdx) const;
	void BeginStructOrClass(const EType& InNewType, const std::vector<std::string>& InLines, 
		const std::vector<std::string>& InWords, const size_t& InLine);
	void ParseLine(const std::vector<std::string>& InLines,
		const std::vector<std::string>& InWords, const size_t& InLine);
	bool IsValidClassOrStruct(const EType& InNewType, const std::vector<std::string>& InLines,
		const size_t& InLine) const;
	void EndCurrentType();

	void ParseProperty(const std::vector<std::string>& InLines,
		const std::vector<std::string>& InWords, const size_t& InLine);

	void OpenNamespace(const std::string& InNamespace);
	void CloseNamespace();
private:
	/** Header ref */
	CHeader* Header;

	/** Pointer to the current structure/class */
	CStruct* CurrentStruct;

	/** Pointer to the current class */
	CClass* CurrentClass;

	/** Path to the header */
	std::string_view Path;

	/** Filename of the header */
	std::string Filename;

	/** Current type processed */
	EType CurrentType;

	/** Current access */
	EAccess CurrentAccess;

	/** Current object name */
	std::string CurrentObjectName;

	/** Nested objects counter */
	uint8_t NestedEncounters;

	/** Current namespace */
	std::string CurrentNamespace;

	uint8_t ScopeCounter;
	std::vector<uint8_t> NamespacesAtScope;
	bool bIgnorePropertiesAndFunctions;
};