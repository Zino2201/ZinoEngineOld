#pragma once

#include "Engine.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include <future>
#include "Shader/ShaderCompiler.h"
#include "Render/UniformBuffer.h"
#include "Renderer/WorldRenderer.h"
#include "Console/Console.h"

namespace ze
{

class ENGINE_API GameApp final : public EngineApp,
	public ticksystem::Tickable
{
public:
	GameApp();

	void post_tick(const float in_delta_time) override;
};

} /* namespace ZE */