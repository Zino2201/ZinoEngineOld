#pragma once

#include "EngineCore.h"
#include <variant>
#include <string>
#include "Delegates/Delegate.h"
#include <algorithm>

namespace ze
{

enum class ConVarFlagBits
{
	None = 0,

	/** Mark this ConVar as a cheat */
	Cheat = 1 << 0,

	/** Allow saving this convar */
	Saved = 1 << 1,
};
ENABLE_FLAG_ENUMS(ConVarFlagBits, ConVarFlags);

/**
 * A console variable
 */
struct ConVar
{
	using ConvarDataType = std::variant<float, int32_t, std::string>;

	enum DataTypeIndex
	{
		DataTypeFloat = 0,
		DataTypeInt32 = 1,
		DataTypeString = 2,
	};

	std::string name;
	ConvarDataType data;

	/** Only for numbers */
	ConvarDataType minimum;

	/** Only for numbers */
	ConvarDataType maximum;

	std::string help;
	ConVarFlags flags;
	
	/** Delegates */
	DelegateNoRet<const ConvarDataType&> on_min_changed;
	DelegateNoRet<const ConvarDataType&> on_max_changed;
	DelegateNoRet<const ConvarDataType&> on_value_changed;

	ConVar(const std::string& in_name, 
		const float& in_default_value,
		const std::string& in_help,
		const ConVarFlags& in_flags = ConVarFlagBits::None) : name(in_name), data(std::in_place_index<0>,
			in_default_value),
			minimum(std::in_place_index<0>, std::numeric_limits<float>::min()),
			maximum(std::in_place_index<0>, std::numeric_limits<float>::max()), help(in_help), 
		flags(in_flags) {}

	ConVar(const std::string& in_name, 
		const int32_t& in_default_value,
		const std::string& in_help,
		const ConVarFlags& in_flags = ConVarFlagBits::None) : name(in_name), data(std::in_place_index<1>, in_default_value),
			minimum(std::in_place_index<1>, std::numeric_limits<int32_t>::min()),
			maximum(std::in_place_index<1>, std::numeric_limits<int32_t>::max()), help(in_help), 
		flags(in_flags) {}

	ConVar(const std::string& in_name, 
		const std::string& in_default_value,
		const std::string& in_help,
		const ConVarFlags& in_flags = ConVarFlagBits::None) : name(in_name), data(std::in_place_index<2>,
			in_default_value), help(in_help), flags(in_flags) {}

	void set_min(const ConvarDataType& in_data)
	{
		minimum = in_data;
		on_min_changed.execute(minimum);
	}

	void set_max(const ConvarDataType& in_data)
	{
		maximum = in_data;
		on_max_changed.execute(maximum);
	}

	void set_float(const float& in_float)
	{
		data = std::clamp<float>(in_float, std::get<float>(minimum), std::get<float>(maximum));
		on_value_changed.execute(data);
	}

	void set_int(const int32_t& in_int)
	{
		data = std::clamp<int32_t>(in_int, std::get<int32_t>(minimum), std::get<int32_t>(maximum));
		on_value_changed.execute(data);
	}

	void set_string(const std::string& in_str)
	{
		data = in_str;
		on_value_changed.execute(data);
	}

	const float& get_min_as_float()
	{
		return std::get<float>(minimum);
	}

	const float& get_max_as_float()
	{
		return std::get<float>(maximum);
	}

	const int32_t& get_min_as_int()
	{
		return std::get<int32_t>(minimum);
	}

	const int32_t& get_max_as_int()
	{
		return std::get<int32_t>(maximum);
	}

	const float& get_as_float()
	{
		return std::get<float>(data);
	}

	const int32_t& get_as_int()
	{
		return std::get<int32_t>(data);
	}

	const std::string& get_as_string()
	{
		return std::get<std::string>(data);
	}
};

}