#pragma once

#include "EngineCore.h"
#include <variant>
#include <string>
#include "Delegates/Delegate.h"
#include <algorithm>

namespace ZE
{

enum class EConVarFlagBits
{
	None = 0,

	/** Mark this ConVar as a cheat */
	Cheat = 1 << 0,

	/** Allow saving this convar */
	Saved = 1 << 1,
};
ENABLE_FLAG_ENUMS(EConVarFlagBits, EConVarFlags);

/**
 * A console variable
 */
struct SConVar
{
	using ConvarDataType = std::variant<float, int32_t, std::string>;

	enum DataTypeIndex
	{
		DataTypeFloat = 0,
		DataTypeInt32 = 1,
		DataTypeString = 2,
	};

	std::string Name;
	ConvarDataType Data;

	/** Only for numbers */
	ConvarDataType Minimum;

	/** Only for numbers */
	ConvarDataType Maximum;

	std::string Help;
	EConVarFlags Flags;
	
	/** Delegates */
	TDelegateNoRet<const ConvarDataType&> OnMinChanged;
	TDelegateNoRet<const ConvarDataType&> OnMaxChanged;
	TDelegateNoRet<const ConvarDataType&> OnValueChanged;

	SConVar(const std::string& InName, 
		const float& InDefaultValue,
		const std::string& InHelp,
		const EConVarFlags& InFlags = EConVarFlagBits::None) : Name(InName), Data(std::in_place_index<0>,
			InDefaultValue), Help(InHelp), Flags(InFlags),
			Minimum(std::in_place_index<0>, std::numeric_limits<float>::min()),
			Maximum(std::in_place_index<0>, std::numeric_limits<float>::max()) {}

	SConVar(const std::string& InName, 
		const int32_t& InDefaultValue,
		const std::string& InHelp,
		const EConVarFlags& InFlags = EConVarFlagBits::None) : Name(InName), Data(std::in_place_index<1>,
			InDefaultValue), Help(InHelp), Flags(InFlags),
			Minimum(std::in_place_index<1>, std::numeric_limits<int32_t>::min()),
			Maximum(std::in_place_index<1>, std::numeric_limits<int32_t>::max()) {}

	SConVar(const std::string& InName, 
		const std::string& InDefaultValue,
		const std::string& InHelp,
		const EConVarFlags& InFlags = EConVarFlagBits::None) : Name(InName), Data(std::in_place_index<2>,
			InDefaultValue), Help(InHelp), Flags(InFlags) {}

	void SetMin(const ConvarDataType& InData)
	{
		Minimum = InData;
		OnMinChanged.Execute(Minimum);
	}

	void SetMax(const ConvarDataType& InData)
	{
		Maximum = InData;
		OnMaxChanged.Execute(Maximum);
	}

	void SetFloat(const float& InFloat)
	{
		Data = std::clamp<float>(InFloat, std::get<float>(Minimum), std::get<float>(Maximum));
		OnValueChanged.Execute(Data);
	}

	void SetInt(const int32_t& InInt)
	{
		Data = std::clamp<int32_t>(InInt, std::get<int32_t>(Minimum), std::get<int32_t>(Maximum));
		OnValueChanged.Execute(Data);
	}

	void SetString(const std::string& InStr)
	{
		Data = InStr;
		OnValueChanged.Execute(Data);
	}

	const float& GetMinAsFloat()
	{
		return std::get<float>(Minimum);
	}

	const float& GetMaxAsFloat()
	{
		return std::get<float>(Maximum);
	}

	const int32_t& GetMinAsInt()
	{
		return std::get<int32_t>(Minimum);
	}

	const int32_t& GetMaxAsInt()
	{
		return std::get<int32_t>(Maximum);
	}

	const float& GetAsFloat()
	{
		return std::get<float>(Data);
	}

	const int32_t& GetAsInt()
	{
		return std::get<int32_t>(Data);
	}

	const std::string& GetAsString()
	{
		return std::get<std::string>(Data);
	}
};

}