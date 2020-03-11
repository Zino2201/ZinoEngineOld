#include "RenderSystemResources.h"

void CRenderSystemResourceFrameCompletedDestruction::FinishDestroy()
{
	if(Resource)
		Resource->FinishDestroy();
}

void IRenderSystemResource::Destroy()
{
	CRenderSystemResourceFrameCompletedDestruction* Destructor =
		new CRenderSystemResourceFrameCompletedDestruction(this);
	Destructor->Destroy();
}