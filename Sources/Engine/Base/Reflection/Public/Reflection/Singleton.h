#pragma once

#include "MinimalMacros.h"

namespace ze::reflection::serialization
{

/**
 * A singleton
 */
template<typename T>
class Singleton
{
public:
	ZE_DLLEXPORT static T& get()
	{
		return get_or_create();
	}
private:
	static T& get_or_create()
	{
		static T instance;
		(void)instance_ref;
		return instance;
	}
private:
	static T& instance_ref;
};

template<typename T> 
T& Singleton<T>::instance_ref = Singleton<T>::get();

}