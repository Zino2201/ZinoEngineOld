#include "Editor/Assets/AssetFactory.h"
#include "Module/ModuleManager.h"
#include "Reflection/Class.h"
#include <robin_hood.h>

namespace ze::editor
{

robin_hood::unordered_set<const ze::reflection::Class*> AddedFactories;
std::vector<std::unique_ptr<CAssetFactory>> Factories;
DelegateHandle ModuleLoadedDelegate;

void ScanForFactories()
{
	for (const auto& Class : 
		ze::reflection::Class::get_derived_classes_from(ze::reflection::Class::get<CAssetFactory>()))
	{
		if (AddedFactories.find(Class) != AddedFactories.end())
			continue;

		OwnerPtr<CAssetFactory> Factory = Class->instantiate<CAssetFactory>();
		Factories.emplace_back(Factory);
		AddedFactories.insert(Class);
	}
}

void OnModuleLoaded(const std::string_view& InName)
{
	ScanForFactories();
}

void InitializeAssetFactoryMgr()
{
	ModuleLoadedDelegate = ze::module::get_on_module_loaded_delegate().bind(&OnModuleLoaded);
	ScanForFactories();
}

void ClearAssetFactoryMgr()
{
	ze::module::get_on_module_loaded_delegate().remove(ModuleLoadedDelegate);
}

CAssetFactory* GetFactoryForFormat(const std::string& InSupportedFormat)
{
	std::string SupportedFormat = InSupportedFormat;
	std::transform(SupportedFormat.begin(), SupportedFormat.end(), SupportedFormat.begin(),
		[](char8_t c) { return std::tolower(c); });

	for (const auto& Factory : Factories)
	{
		for (auto Format : Factory->GetSupportedFormats())
		{
			std::transform(Format.begin(), Format.end(), Format.begin(),
				[](char8_t c) { return std::tolower(c); });

			if (SupportedFormat == Format)
				return Factory.get();
		}
	}

	return nullptr;
}

}