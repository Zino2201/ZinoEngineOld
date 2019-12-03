#pragma once

#include "EngineCore.h"
#include "Asset.h"
#include <future>

enum class EAssetLoadMethod
{
	Async,
	Sync
};

class IAsset;

class CAssetManager
{
public:
	/**
	 * Get an asset
	 * Path relative to Assets directory
	 * If assets null, load it
	 */
	template<typename T>
	std::shared_ptr<T> Get(const std::string& InPath,
		const EAssetLoadMethod& InAssetLoadMethod = EAssetLoadMethod::Sync)
	{
		std::string RealPath = "Assets/" + InPath;

		/** First try to search if asset is already loaded */
		auto FindResult = Assets.find(InPath);
		if (FindResult != Assets.end())
		{
			return std::static_pointer_cast<T>(FindResult->second);
		}
		else
		{
			if(InAssetLoadMethod != EAssetLoadMethod::Sync) 
				LOG(ELogSeverity::Error, "Unimplemented")

			auto Ret =
				Assets.insert(std::make_pair(InPath, std::make_shared<T>()));
			Ret.first->second->Load(RealPath);
			return std::static_pointer_cast<T>(Ret.first->second);
		}
	}
private:
	std::map<std::string, std::shared_ptr<IAsset>> Assets;
};