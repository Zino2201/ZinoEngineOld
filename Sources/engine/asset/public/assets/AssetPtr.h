#pragma once

#include <filesystem>

namespace ze
{

/**
 * Weak pointer to an maybe loaded asset
 */
class AssetPtr
{
private:
	std::filesystem::path path;
};

}