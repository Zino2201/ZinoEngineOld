#pragma once

#include "EngineCore.h"
#include "Delegates/Delegate.h"
#include <variant>

namespace ze::ui
{

template<typename T>
class Attribute
{
	struct ConstantGetter
	{
		T data;
	};

	struct FunctionGetter
	{
		Delegate<T&> delegate;
	};
	
	struct Visitor
	{
		T& operator()(ConstantGetter& getter) 
		{
			return getter.data;
		}

		T& operator()(FunctionGetter& getter) 
		{
			return getter.delegate.execute();
		}
	};
public:
	Attribute() : getter(ConstantGetter()) {}
	Attribute(const T& in_value) { set(in_value); }

	void set(const T& in_value)
	{
		getter = ConstantGetter { in_value };
	}

	ZE_FORCEINLINE const T& get() 
	{ 
		return std::visit(Visitor(), getter); 
	}
private:
	std::variant<ConstantGetter, FunctionGetter> getter;
};

}