#pragma once

#include "VulkanCore.h"

class CVulkanDevice;

class CVulkanDeviceResource
{
public:
	CVulkanDeviceResource(CVulkanDevice* InDevice) : Device(InDevice) {}
	virtual ~CVulkanDeviceResource() { } 
protected:
	CVulkanDevice* Device;
};