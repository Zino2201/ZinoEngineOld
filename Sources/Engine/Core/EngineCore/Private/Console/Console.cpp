#pragma once

#include "Console/Console.h"
#include <charconv>

namespace ZE
{

void CConsole::Execute(const std::string_view& InCmdName, 
	const std::vector<std::string_view>& InParams)
{
	/** Search for convars */
	for(SConVar& ConVar : ConVars)
	{
		if(ConVar.Name == InCmdName)
		{
			if(InParams.empty())
			{
				if(ConVar.Data.index() == SConVar::DataTypeFloat)
					ZE::Logger::Error("Invalid syntax.\n{}\n\t- Min: {}\n\t- Max: {}\n\t- Current: {}", 
						ConVar.Help.c_str(),
						ConVar.GetMinAsFloat(), ConVar.GetMaxAsFloat(),
						ConVar.GetAsFloat());
				else if (ConVar.Data.index() == SConVar::DataTypeInt32)
					ZE::Logger::Error( 
						"Invalid syntax\n{}\n\t- Min: {}\n\t- Max: {}\n\t- Current: {}", 
						ConVar.Help.c_str(),
						ConVar.GetMinAsInt(), ConVar.GetMaxAsInt(),
						ConVar.GetAsInt());
				else
					ZE::Logger::Error("Invalid syntax\n{}\n\t- Current:", 
						ConVar.Help.c_str(),
						ConVar.GetAsString().c_str());
			}
			else
			{
				const auto& Arg = InParams[0];
				switch(ConVar.Data.index())
				{
				case SConVar::DataTypeInt32:
				{
					int32_t Int = 0;
					auto Result = std::from_chars(Arg.data(), Arg.data() + Arg.size(), Int);
					if(Result.ec == std::errc::invalid_argument)
					{
						ZE::Logger::Error("Invalid argument \"{}\"", InParams[0].data());
						return;
					}
					else
						ConVar.SetInt(Int);

					break;
				}
				case SConVar::DataTypeFloat:
				{
					float Float = 0;
					auto Result = std::from_chars(Arg.data(), Arg.data() + Arg.size(), Float);
					if (Result.ec == std::errc::invalid_argument)
					{
						ZE::Logger::Error("Invalid argument \"{}\"", InParams[0].data());
						return;
					}
					else
						ConVar.SetFloat(Float);
					break;
				}
				case SConVar::DataTypeString:
					ConVar.SetString(InParams[0].data());
					break;
				}

				ZE::Logger::Info("\"{}\" changed to \"{}\"", InCmdName.data(),
					InParams[0].data());
			}
			return;
		}
	}

	ZE::Logger::Error("Unknown concmd/convar \"{}\"", InCmdName.data());
}

}