#pragma once

#include "EngineCore.h"
#include "BackendInfo.h"

namespace ze::gfx
{

class Backend;

/**
 * Find all installed backends and register them
 */
void find_backends();
const std::vector<BackendInfo>& get_backends();

/**
 * Try to create the provided backend with the request shader model
 * \param in_backend Backend to create
 * \param in_requested_shader_model Shader model requested, if not supported then the backend creation MUST fail
 * \return True if the backend has been created, in this case you can call get_running_backend().
 * Backend creation can fail if the backend:
 *  - Does not support the requested shader model
 *  - The user's hardware does not support the backend
 */
bool create_backend(const BackendInfo* in_backend, const ShaderModel in_requested_shader_model);
Backend* get_running_backend();

/**
 * Destroy the running backend (if it exists)
 */
void destroy_running_backend();

/**
 * Get the current shader model we use
 */
ShaderModel get_current_shader_model();
	
}