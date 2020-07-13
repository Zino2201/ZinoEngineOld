#include "Engine/Engine.h"
#include "Module/Module.h"
#include "Module/ModuleManager.h"
#include "Engine/TickSystem.h"
#include "Engine/ECS.h"
#include "Profiling/Profiling.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, Engine)

void CEngine::Initialize()
{
	/** Initialize ECS */
	ECS::CECSManager::Get().Initialize();
}

void CEngine::Tick(union SDL_Event* InEvent, const float& InDeltaTime)
{

}

void CEngine::Draw()
{

}


} /* namespace ZE */