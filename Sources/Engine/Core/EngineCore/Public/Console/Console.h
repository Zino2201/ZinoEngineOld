#pragma once

#include "ConVar.h"
#include "NonCopyable.h"

namespace ZE
{

/**
 * Main console singleton
 */
class ENGINECORE_API CConsole : public CNonCopyable
{
public:
	static CConsole& Get()
	{
		static CConsole Instance;
		return Instance;
	}
	
	template<typename... Args>
	size_t EmplaceConVar(Args&&... InArgs)
	{
		ConVars.emplace_back(std::forward<Args>(InArgs)...);
		return ConVars.size() - 1;
	}

	void Execute(const std::string_view& InCmdName, const std::vector<std::string_view>& InParams);

	SConVar& GetConVar(const size_t& InIdx) { return ConVars[InIdx]; }
private:
	/** Coherent array of convars */
	std::vector<SConVar> ConVars;
};
/**
 * Type trait that return true if the type can be used as a number for convars
 */
template<typename T>
constexpr bool TIsValidConVarNumber = std::is_same_v<T, float> || std::is_same_v<T, int32_t>;

/**
 * Helper type to make convar creation & manipulation easier
 */
template<typename T>
class TConVar
{
public:
	TConVar(const std::string& InName, 
		const T& InDefaultValue,
		const std::string& InHelp,
		const EConVarFlags& InFlags = EConVarFlagBits::None) :
		ConVar(CConsole::Get().EmplaceConVar(InName, InDefaultValue, InHelp, InFlags)) {}
	
	/**
	 * Constructor with min/max, only works for valid convars numbers
	 */
	TConVar(const std::string& InName,
		const T& InDefaultValue,
		const std::string& InHelp, 
		const T& InMin,
		const T& InMax,
		const EConVarFlags& InFlags = EConVarFlagBits::None) requires TIsValidConVarNumber<T> :
		ConVar(CConsole::Get().EmplaceConVar(InName, InDefaultValue, InHelp, InFlags)) 
	{
		CConsole::Get().GetConVar(ConVar).SetMin(InMin);
		CConsole::Get().GetConVar(ConVar).SetMax(InMax);
	}

	/**
	 * Call the specified function when the convar is changed
	 */
	void BindOnChanged(void(*InFunc)(const T& InData))
	{
		CConsole::Get().GetConVar(ConVar).OnValueChanged.Bind(
			[=](const SConVar::ConvarDataType& InData)
			{
				(*InFunc)(std::get<T>(InData));
			});
	}

	/**
	 * Same as above but supported member function
	 */
	template<typename Binder>
	void BindOnChanged(Binder* InBinder, void(Binder::*InFunc)(const T& InData))
	{
		CConsole::Get().GetConVar(ConVar).OnValueChanged.Bind(
			[=](const SConVar::ConvarDataType& InData)
			{
				(InBinder->*InFunc)(std::get<T>(InData));
			});
	}

	const T& Get()
	{
		if constexpr(std::is_same_v<T, float>)
			return GetAsFloat();
		else if constexpr(std::is_same_v<T, int32_t>)
			return GetAsInt();
		else
			return GetAsString();
	}

	const T& GetMin()
	{
		return std::get<T>(CConsole::Get().GetConVar(ConVar).Minimum);
	}

	const T& GetMax()
	{
		return std::get<T>(CConsole::Get().GetConVar(ConVar).Maximum);
	}

	const float& GetAsFloat()
	{
		return CConsole::Get().GetConVar(ConVar).GetAsFloat();
	}

	const int32_t& GetAsInt()
	{
		return CConsole::Get().GetConVar(ConVar).GetAsInt();
	}

	const std::string& GetAsString()
	{
		if constexpr(std::is_same_v<T, float>)
			return std::to_string(GetAsFloat());
		else if constexpr(std::is_same_v<T, int32_t>)
			return std::to_string(GetAsInt());
		else
			return CConsole::Get().GetConVar(ConVar).GetAsString();
	}
private:
	size_t ConVar;
};

}