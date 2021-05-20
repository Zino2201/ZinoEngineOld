#include "StringUtil.h"

namespace ze::stringutil
{

std::vector<std::string> split(const std::string& string, 
	const char& delimiter)
{
	std::vector<std::string> tokens;
	tokens.reserve(5);

	std::stringstream stream(string);
	std::string token;
	while (std::getline(stream, token, delimiter)) 
	{
		if(!token.empty())
			tokens.push_back(std::move(token));
	}

	return tokens;
}

}