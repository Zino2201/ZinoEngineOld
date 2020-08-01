#pragma once

namespace ZE
{

/**
 * A singlecast delegate
 */
template<typename Ret, typename... Args>
class TDelegate
{
	using TSignature = Ret(Args...);

public:
	TDelegate() {}
	template<typename Lambda>
	TDelegate(Lambda&& InFunction)
		: Function(InFunction) {}

	/**
	 * Bind a function to delegate
	 */
	void Bind(const std::function<TSignature>& InFunction)
	{
		Function = InFunction;
	}

	/**
	 * Call all functions
	 */
	Ret Execute(Args&&... InArgs) const
	{
		if(Function)
		{
			if constexpr(!std::is_void_v<Ret>)
				return Function(std::forward<Args>(InArgs)...);
			else
				Function(std::forward<Args>(InArgs)...);
		}
		else
		{
			if constexpr(!std::is_void_v<Ret>)
				return {};
		}
	}

	operator bool() const
	{
		return Function != nullptr;
	}
private:
	std::function<TSignature> Function;
};

template<typename... Args>
using TDelegateNoRet = TDelegate<void, Args...>;

}