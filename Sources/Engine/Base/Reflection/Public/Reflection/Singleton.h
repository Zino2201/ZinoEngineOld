#pragma once

#include "MinimalMacros.h"
#include "NonCopyable.h"

namespace ZE
{

/**
 * A singleton
 */
template<typename T>
class TSingleton : public CNonCopyable
{
public:
	ZE_DLLEXPORT static T& Get()
	{
		return GetOrCreate();
	}
private:
	static T& GetOrCreate()
	{
		static T Instance;
		(void)InstanceRef;
		return Instance;
	}
private:
	static T& InstanceRef;
};

template<typename T> 
T& TSingleton<T>::InstanceRef = TSingleton<T>::Get();

}