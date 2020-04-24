#include "Engine/Engine.h"
#include "Module/Module.h"
#include "Engine/Entity.h"
#include "Module/ModuleManager.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, "Engine")

void CEngine::Initialize()
{
	ZE::CModuleManager::LoadModule("ECS");
}

void CEngine::Tick(union SDL_Event* InEvent, const float& InDeltaTime)
{

}

} /* namespace ZE */