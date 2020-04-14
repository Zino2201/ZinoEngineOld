#include "Engine/Engine.h"
#include "Module/Module.h"
#include "Engine/Entity.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, "Engine")

void CEngine::Initialize()
{
	//ECSManager->Initialize();
}

void CEngine::Tick(union SDL_Event* InEvent, const float& InDeltaTime)
{

}

} /* namespace ZE */