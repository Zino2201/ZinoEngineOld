#include "Engine/Assets/Texture.h"
#if ZE_WITH_EDITOR
#include "AssetDataCache/AssetDataCache.h"
#include "Assets/AssetCooker.h"
#endif

namespace ze
{

void TexturePlatformData::load(Texture* in_texture)
{
	switch(in_texture->get_asset_format())
	{
	case Texture::TextureAssetFormat::TextureEditor:
#if ZE_WITH_EDITOR
		size_t i = 0;
		/** Check for missing mipmaps */
		bool missing_mipmap = false;
		for (auto& mip : mipmaps)
		{
			std::string key = uuids::to_string(in_texture->get_uuid()) + "_"
				+ std::to_string(in_texture->get_compression_mode()) + "_"
				+ std::to_string(in_texture->get_format()) + "_"
				+ "Mip_" + std::to_string(i++) + "_"
				+ std::to_string(mip.width) + "_"
				+ std::to_string(mip.height) + "_"
				+ std::to_string(mip.depth);
			if (!assetdatacache::has_key(key))
			{
				missing_mipmap = true;
				break;
			}
		}

		/** Recompute the mipmaps if missing, this will save this to the assetdatacache */
		if(missing_mipmap)
		{
			AssetCooker::cook_asset(in_texture);
		}

		i = 0;
		for(auto& mip : mipmaps)
		{
			std::string key = uuids::to_string(in_texture->get_uuid()) + "_"
				+ std::to_string(in_texture->get_compression_mode()) + "_"
				+ std::to_string(in_texture->get_format()) + "_"
				+ "Mip_" + std::to_string(i++) + "_"
				+ std::to_string(mip.width) + "_"
				+ std::to_string(mip.height) + "_"
				+ std::to_string(mip.depth);
			mip.data = assetdatacache::get_sync(key);
		}
#endif
		break;
	}
}

}