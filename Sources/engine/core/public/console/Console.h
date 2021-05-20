#pragma once

#include "ConVar.h"
#include "NonCopyable.h"

namespace ze
{

/**
 * Main console singleton
 */
class CORE_API CConsole : public CNonCopyable
{
public:
	CConsole();
	~CConsole();
	static CConsole& Get();
	
	template<typename... Args>
	size_t EmplaceConVar(Args&&... InArgs)
	{
		ConVars.emplace_back(std::forward<Args>(InArgs)...);
		ConVars.back().default_value = ConVars.back().data;
		return ConVars.size() - 1;
	}

	void Execute(const std::string_view& InCmdName, const std::vector<std::string_view>& InParams);

	ConVar& GetConVar(const size_t& InIdx) { return ConVars[InIdx]; }
	auto& get_convars() { return ConVars; }
private:
	/** Coherent array of convars */
	std::vector<ConVar> ConVars;
};
/**
 * Type trait that return true if the type can be used as a number for convars
 */
template<typename T>
constexpr bool IsValidConVarNumber = std::is_same_v<T, float> || std::is_same_v<T, int32_t>;

/**
 * Helper type to make convar creation & manipulation easier
 */
template<typename T>
class ConVarRef
{
public:
	ConVarRef(const std::string& in_name, 
		const T& in_default_value,
		const std::string& in_help,
		const ConVarFlags& in_flags = ConVarFlagBits::None) :
		idx(CConsole::Get().EmplaceConVar(in_name, in_default_value, in_help, in_flags)) {}
	
	/**
	 * Constructor with min/max, only works for valid convars numbers
	 */
	ConVarRef(const std::string& in_name,
		const T& in_default_value,
		const std::string& in_help, 
		const T& in_min,
		const T& in_max,
		const ConVarFlags& in_flags = ConVarFlagBits::None) requires IsValidConVarNumber<T> :
		idx(CConsole::Get().EmplaceConVar(in_name, in_default_value, in_help, in_flags)) 
	{
		get_convar().set_min(in_min);
		get_convar().set_max(in_max);
	}

	ZE_FORCEINLINE ConVar& get_convar()
	{
		return CConsole::Get().GetConVar(idx);
	}

	/**
	 * Call the specified function when the convar is changed
	 */
	void bind_on_changed(void(*in_func)(const T& in_data))
	{
		get_convar().on_value_changed.bind(
			[=](const ConVar::ConvarDataType& in_data)
			{
				(*in_func)(std::get<T>(in_data));
			});
	}

	/**
	 * Same as above but with support for member functions
	 */
	template<typename Binder>
	void BindOnChanged(Binder* binder, void(Binder::*func)(const T& in_data))
	{
		get_convar().on_value_changed.bind(
			[=](const ConVar::ConvarDataType& data)
			{
				(binder->*func)(std::get<T>(data));
			});
	}

	ZE_FORCEINLINE const T& get()
	{
		if constexpr(std::is_same_v<T, float>)
			return get_as_float();
		else if constexpr(std::is_same_v<T, int32_t>)
			return get_as_int();
		else
			return get_as_string();
	}

	ZE_FORCEINLINE const T& get_min()
	{
		return std::get<T>(get_convar().minimum);
	}

	ZE_FORCEINLINE const T& get_max()
	{
		return std::get<T>(get_convar().maximum);
	}

	ZE_FORCEINLINE const float& get_as_float()
	{
		return get_convar().get_as_float();
	}

	ZE_FORCEINLINE const int32_t& get_as_int()
	{
		return get_convar().get_as_int();
	}

	ZE_FORCEINLINE const std::string& get_as_string()
	{
		if constexpr(std::is_same_v<T, float>)
			return std::to_string(get_as_float());
		else if constexpr(std::is_same_v<T, int32_t>)
			return std::to_string(get_as_int());
		else
			return get_convar().get_as_string();
	}
private:
	size_t idx;
};

}