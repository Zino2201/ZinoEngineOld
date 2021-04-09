#pragma once

#include "Assertions.h"
#include <functional>

namespace ze
{

/**
 * A singlecast delegate
 */
template<typename Ret, typename... Args>
class Delegate
{
	using Signature = Ret(Args...);

public:
	Delegate() {}
	
	template<typename Func>
	Delegate(Func&& in_func)
		: function(in_func) {}

	/**
	 * Bind a function to delegate
	 */
	void bind(const std::function<Signature>& in_func)
	{
		function = in_func;
	}

	void bind(std::function<Signature>&& in_func)
	{
		function = in_func;
	}

	/**
	 * Call all functions
	 */
	Ret execute(Args&&... args) const
	{
		if(function)
		{
			if constexpr(!std::is_void_v<Ret>)
				return function(std::forward<Args>(args)...);
			else
				function(std::forward<Args>(args)...);
		}
		else
		{
			if constexpr(std::is_void_v<Ret>)
				return;
			else if constexpr(!std::is_reference_v<Ret>)
				return {};

			ZE_CHECK(false);
		}
	}

	void clear()
	{
		function = nullptr;
	}

	operator bool() const
	{
		return function != nullptr;
	}
private:
	std::function<Signature> function;
};

template<typename... Args>
using DelegateNoRet = Delegate<void, Args...>;

}