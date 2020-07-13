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
					LOG(ELogSeverity::Error, None, 
						"Invalid syntax.\n%s\n\t- Min: %f\n\t- Max: %f\n\t- Current: %f", 
						ConVar.Help.c_str(),
						ConVar.GetMinAsFloat(), ConVar.GetMaxAsFloat(),
						ConVar.GetAsFloat());
				else if (ConVar.Data.index() == SConVar::DataTypeInt32)
					LOG(ELogSeverity::Error, None, 
						"Invalid syntax\n%s\n\t- Min: %d\n\t- Max: %d\n\t- Current: %d", 
						ConVar.Help.c_str(),
						ConVar.GetMinAsInt(), ConVar.GetMaxAsInt(),
						ConVar.GetAsInt());
				else
					LOG(ELogSeverity::Error, None, "Invalid syntax\n%s\n\t- Current:", 
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
						LOG(ELogSeverity::Error, None, "Invalid argument \"%s\"", InParams[0].data());
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
						LOG(ELogSeverity::Error, None, "Invalid argument \"%s\"", InParams[0].data());
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

				LOG(ELogSeverity::Info, None, "\"%s\" changed to \"%s\"", InCmdName.data(),
					InParams[0].data());
			}
			return;
		}
	}

	LOG(ELogSeverity::Error, None, "Unknown concmd/convar \"%s\"", InCmdName.data());
}

}