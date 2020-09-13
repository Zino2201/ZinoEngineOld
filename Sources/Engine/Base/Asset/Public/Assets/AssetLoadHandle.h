#pragma once

namespace ZE
{

/**
 * Handle to an asset load operation
 * As long as the handle is alive, the assets will stay in memory
 * A handle can own multiple assets
 */
class CAssetLoadHandle
{
public:
private:
	std::vector<std::shared_ptr<CAsset>> Assets;
};

}