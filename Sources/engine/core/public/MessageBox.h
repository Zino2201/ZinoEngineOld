#pragma once

#include "EngineCore.h"
#include <string>

namespace ze
{

enum class MessageBoxButtonFlagBits
{
	None = 0,

	Ok = 1 << 0,
	OkCancel = 1 << 1
};
ENABLE_FLAG_ENUMS(MessageBoxButtonFlagBits, MessageBoxButtonFlags);

enum class MessageBoxIcon
{
	None,
	Question,
	Information,
	Warning,
	Critical
};

/**
 * Portable way to display a message box
 */
CORE_API int message_box(const std::string& in_title, const std::string& in_text, const MessageBoxButtonFlags& in_button_flags,
	const MessageBoxIcon& in_icon);

}