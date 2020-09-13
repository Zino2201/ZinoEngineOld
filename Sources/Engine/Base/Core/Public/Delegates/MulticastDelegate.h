#pragma once

#include <vector>
#include <functional>

namespace ZE
{

/**
 * Simple multicast delegate
 */
template<typename Ret, typename... Args>
class TMulticastDelegate
{
	using TSignature = Ret(Args...);

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

template<typename... Args>
using TMulticastDelegateNoRet = TMulticastDelegate<void, Args...>;

} /* namespace ZE */