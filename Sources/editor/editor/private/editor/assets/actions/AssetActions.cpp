#include "editor/assets/AssetActions.h"
#include "module/ModuleManager.h"
#include "reflection/Class.h"
#include <robin_hood.h>

namespace ze::editor
{

robin_hood::unordered_set<const ze::reflection::Class*> added_actions;
std::vector<std::unique_ptr<AssetActions>> actions;
DelegateHandle asset_actions_module_loaded_delegate;

void scan_for_asset_actions()
{
	for (const auto& Class :
		ze::reflection::Class::get_derived_classes_from(ze::reflection::Class::get<AssetActions>()))
	{
		if (added_actions.find(Class) != added_actions.end())
			continue;

		OwnerPtr<AssetActions> factory = Class->instantiate<AssetActions>();
		actions.emplace_back(factory);
		added_actions.insert(Class);
	}
}

void on_asset_actions_module_loaded(const std::string_view& InName)
{
	scan_for_asset_actions();
}

void initialize_asset_actions_mgr()
{
	asset_actions_module_loaded_delegate = ze::module::get_on_module_loaded_delegate().bind(&on_asset_actions_module_loaded);
	scan_for_asset_actions();
}

void destroy_asset_actions_mgr()
{
	ze::module::get_on_module_loaded_delegate().remove(asset_actions_module_loaded_delegate);
}

AssetActions* get_actions_for(const reflection::Class* in_class)
{
	for(const auto& action : actions)
	{
		if(action->get_supported_class() == in_class)
			return action.get();
	}

	return nullptr;
}

}