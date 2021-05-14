#include "PlatformMgr.h"
#include "Module/Module.h"

namespace ze
{

class PlatformWindowsModule : public module::Module
{
public:
	PlatformWindowsModule()
	{
#if ZE_WITH_EDITOR
		register_windows_platform(true);
		set_current_platform("WindowsEditor");
#endif
		register_windows_platform(false);
	}
private:
	void register_windows_platform(bool is_editor)
	{
		PlatformInfo p_info;
		p_info.os = PlatformOS::Windows;
		p_info.env = is_editor ? PlatformEnvironnement::Editor : PlatformEnvironnement::Game;
		p_info.name = is_editor ? "WindowsEditor" : "WindowsGame";
		p_info.is_little_endian = std::endian::native == std::endian::little;

		register_platform(p_info);
	}
};

}

ZE_DEFINE_MODULE(ze::PlatformWindowsModule, PlatformWindows);