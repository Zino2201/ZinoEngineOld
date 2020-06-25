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
	/** Tick CTickables */
	CTickSystem::Get().Tick(ETickOrder::StartOfFrame, InDeltaTime);
	CTickSystem::Get().Tick(ETickOrder::PostPhysics, InDeltaTime);
	CTickSystem::Get().Tick(ETickOrder::EndOfFrame, InDeltaTime);
}

} /* namespace ZE */