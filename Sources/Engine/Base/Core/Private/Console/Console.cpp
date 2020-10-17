#include "Console/Console.h"
#include <charconv>

namespace ze
{

void CConsole::Execute(const std::string_view& InCmdName, 
	const std::vector<std::string_view>& InParams)
{
	/** Search for convars */
	for(ConVar& ConVar : ConVars)
	{
		if(ConVar.name == InCmdName)
		{
			if(InParams.empty())
			{
				if(ConVar.data.index() == ConVar::DataTypeFloat)
					ze::logger::error("Invalid syntax.\n{}\n\t- Min: {}\n\t- Max: {}\n\t- Current: {}", 
						ConVar.help.c_str(),
						ConVar.get_min_as_float(), ConVar.get_max_as_float(),
						ConVar.get_max_as_float());
				else if (ConVar.data.index() == ConVar::DataTypeInt32)
					ze::logger::error( 
						"Invalid syntax\n{}\n\t- Min: {}\n\t- Max: {}\n\t- Current: {}", 
						ConVar.help.c_str(),
						ConVar.get_min_as_int(), ConVar.get_max_as_int(),
						ConVar.get_as_int());
				else
					ze::logger::error("Invalid syntax\n{}\n\t- Current:", 
						ConVar.help.c_str(),
						ConVar.get_as_string().c_str());
			}
			else
			{
				const auto& Arg = InParams[0];
				switch(ConVar.data.index())
				{
				case ConVar::DataTypeInt32:
				{
					int32_t Int = 0;
					auto Result = std::from_chars(Arg.data(), Arg.data() + Arg.size(), Int);
					if(Result.ec == std::errc::invalid_argument)
					{
						ze::logger::error("Invalid argument \"{}\"", InParams[0].data());
						return;
					}
					else
						ConVar.set_int(Int);

					break;
				}
				case ConVar::DataTypeFloat:
				{
					// For some reasons
					// Clang doesn't have std::from_chars for floats
					float Float = static_cast<float>(std::atof(Arg.data()));

					/*if (Result.ec == std::errc::invalid_argument)
					{
						ze::logger::error("Invalid argument \"{}\"", InParams[0].data());
						return;
					}
					else*/
						ConVar.set_float(Float);
					break;
				}
				case ConVar::DataTypeString:
					ConVar.set_string(InParams[0].data());
					break;
				}

				ze::logger::info("\"{}\" changed to \"{}\"", InCmdName.data(),
					InParams[0].data());
			}
			return;
		}
	}

	ze::logger::error("Unknown concmd/convar \"{}\"", InCmdName.data());
}

}