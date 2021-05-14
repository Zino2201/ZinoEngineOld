#pragma once

#include "Platform.h"

namespace ze
{

void register_platform(const PlatformInfo& in_info);
void set_current_platform(const std::string& name);
PlatformInfo& get_current_platform();

}