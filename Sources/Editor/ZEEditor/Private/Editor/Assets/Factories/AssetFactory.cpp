#include "Editor/Assets/AssetFactory.h"
#include "Module/ModuleManager.h"
#include "Reflection/Class.h"
#include <robin_hood.h>

namespace ze::editor
{

AssetFactory::AssetFactory() : asset_class(nullptr), can_be_instantiated(false), asset_file_extension("zeasset") {}

robin_hood::unordered_set<const ze::reflection::Class*> added_factories;
std::vector<std::unique_ptr<AssetFactory>> factories;
DelegateHandle module_loaded_delegate;

void scan_for_factories()
{
	for (const auto& Class : ze::reflection::Class::get_derived_classes_from(ze::reflection::Class::get<AssetFactory>()))
	{
		if (added_factories.find(Class) != added_factories.end())
			continue;

		OwnerPtr<AssetFactory> factory = Class->instantiate<AssetFactory>();
		factories.emplace_back(factory);
		added_factories.insert(Class);
	}
}

void on_module_loaded(const std::string_view& InName)
{
	scan_for_factories();
}

void initialize_asset_factory_mgr()
{
	module_loaded_delegate = ze::module::get_on_module_loaded_delegate().bind(&on_module_loaded);
	scan_for_factories();
}

void destroy_asset_factory_mgr()
{
	ze::module::get_on_module_loaded_delegate().remove(module_loaded_delegate);
}

AssetFactory* get_factory_for_format(const std::string& in_supported_format)
{
	std::string supported_format = in_supported_format;
	std::transform(supported_format.begin(), supported_format.end(), supported_format.begin(),
		[](char8_t c) { return std::tolower(c); });

	for (const auto& factory : factories)
	{
		for (auto format : factory->get_supported_formats())
		{
			std::transform(format.begin(), format.end(), format.begin(),
				[](char8_t c) { return std::tolower(c); });

			if (supported_format == format)
				return factory.get();
		}
	}

	return nullptr;
}

const std::vector<std::unique_ptr<AssetFactory>>& get_factories()
{
	return factories;
}

}