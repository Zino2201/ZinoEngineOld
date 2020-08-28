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
	using TSignature = void(Args...);

public:
	/**
	 * Bind a function to delegate
	 */
	void Bind(const std::function<TSignature>& InFunction)
	{
		Functions.push_back(InFunction);
	}

	/**
	 * Call all functions
	 */
	void Broadcast(Args... InArgs)
	{
		for (const auto& Function : Functions)
			Function(std::forward<Args>(InArgs)...);
	}
private:
	std::vector<std::function<TSignature>> Functions;
};

} /* namespace ZE */