#pragma once

#include "EngineCore.h"

namespace ZE
{

/**
 * A weak pointer to an asset
 * Maybe nullptr if the asset is not loaded
 */
template<typename T>
class TAssetPtr
{
public:
	TAssetPtr() : Path("") {}
	TAssetPtr(std::nullptr_t) : Path("") {}

	ZE_FORCEINLINE operator bool() const { return Ptr; }

	ZE_FORCEINLINE T* operator->() { return Ptr.get(); }
	ZE_FORCEINLINE const T* operator->() const { return Ptr.get(); }

	ZE_FORCEINLINE T* Get()
	{
		
	}
private:
	/** Path to the asset */
	std::string Path;
};

}