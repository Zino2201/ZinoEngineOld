#pragma once

#include "Platform.h"

namespace ze
{

/**
 * Find platforms in Config/Platforms directory
 */
void find_platforms();
void register_platform(const PlatformInfo& in_info);
PlatformInfo& get_running_platform();

}