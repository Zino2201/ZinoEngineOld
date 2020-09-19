#pragma once

#include <vector>
#include <functional>

namespace ZE
{

using DelegateHandle = size_t;

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
	DelegateHandle Bind(const std::function<TSignature>& InFunction)
	{
		Functions.push_back(InFunction);
		return Functions.size() - 1;
	}

	/**
	 * Call all functions
	 */
	void Broadcast(Args... InArgs)
	{
		for (const auto& Function : Functions)
			Function(std::forward<Args>(InArgs)...);
	}

	/**
	 * Remove the function from the delegate
	 */
	void Remove(const DelegateHandle& InFunction)
	{
		verify(InFunction < Functions.size());

		Functions.erase(Functions.begin() + InFunction);
	}
private:
	std::vector<std::function<TSignature>> Functions;
};

template<typename... Args>
using TMulticastDelegateNoRet = TMulticastDelegate<void, Args...>;

} /* namespace ZE */