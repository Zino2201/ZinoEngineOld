#pragma once

#include <vector>
#include <functional>

namespace ZE
{

/**
 * Simple multicast delegate
 */
template<typename... Args>
class TMulticastDelegate
{
public:
	/**
	 * Bind a function to delegate
	 */
	void Bind(const std::function<void(Args...)>& InFunction)
	{
		Functions.emplace_back(InFunction);
	}

	/**
	 * Call all functions
	 */
	void Broadcast(Args&&... InArgs)
	{
		for (const std::function<void(Args...)>& Function : Functions)
			Function(InArgs...);
	}
private:
	std::vector<std::function<void(Args...)>> Functions;
};

} /* namespace ZE */