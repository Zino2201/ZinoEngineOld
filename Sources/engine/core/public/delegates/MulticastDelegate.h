#pragma once

#include <vector>
#include <functional>

namespace ze
{

using DelegateHandle = size_t;

/**
 * Simple multicast delegate
 */
template<typename Ret, typename... Args>
class MulticastDelegate
{
	using Signature = Ret(Args...);

public:
	/**
	 * Bind a function to delegate
	 */
	DelegateHandle bind(const std::function<Signature>& func)
	{
		functions.push_back(func);
		return functions.size() - 1;
	}

	/**
	 * Call all functions
	 */
	void broadcast(Args... args)
	{
		for (const auto& function : functions)
			function(std::forward<Args>(args)...);
	}

	/**
	 * Remove the function from the delegate
	 */
	void remove(const DelegateHandle& handle)
	{
		ZE_CHECK(handle < functions.size());
		functions.erase(functions.begin() + handle);
	}
private:
	std::vector<std::function<Signature>> functions;
};

template<typename... Args>
using MulticastDelegateNoRet = MulticastDelegate<void, Args...>;

} /* namespace ZE */