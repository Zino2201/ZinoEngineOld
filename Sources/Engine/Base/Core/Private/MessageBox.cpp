#include "MessageBox.h"
#if ZE_PLATFORM(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace ze
{

int message_box(const std::string& in_title, const std::string& in_text, const MessageBoxButtonFlags& in_button_flags,
	const MessageBoxIcon& in_icon)
{
#if ZE_PLATFORM(WINDOWS)
	UINT type = 0;

	if(in_button_flags & MessageBoxButtonFlagBits::Ok)
		type |= MB_OK;

	if(in_button_flags & MessageBoxButtonFlagBits::OkCancel)
		type |= MB_OKCANCEL;

	switch(in_icon)
	{
	case MessageBoxIcon::Question:
		type |= MB_ICONQUESTION;
		break;
	default:
	case MessageBoxIcon::Information:
		type |= MB_ICONINFORMATION;
		break;
	case MessageBoxIcon::Warning:
		type |= MB_ICONWARNING;
		break;
	case MessageBoxIcon::Critical:
		type |= MB_ICONERROR;
		break;
	}

	return MessageBoxA(nullptr, in_text.c_str(), 
		in_title.c_str(), type);
#endif
}

}