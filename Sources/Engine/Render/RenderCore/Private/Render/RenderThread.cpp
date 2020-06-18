#include "Module/Module.h"
#include "Render/RenderThread.h"
#include <SDL2/SDL.h>

namespace ZE
{

DEFINE_MODULE(CDefaultModule, RenderCore)

CRenderThread::CRenderThread() = default;

void CRenderThread::Run(CSemaphore* InGameThreadSemaphore)
{
	GameThreadSemaphore = InGameThreadSemaphore;
	bRun = true;

	while(bRun)
	{
		/** Execute commands */
		if(!Commands.empty())
		{
			CommandsMutex.lock();
			while(!Commands.empty())
			{
				auto& Command = Commands.front();
				if(Command)
				{
					Command->Execute();
					delete Command;
				}
				Commands.pop();
			}

			CommandsMutex.unlock();
			CommandsExecutedSemaphore.Notify();
		}

		RenderThreadFrameFinishedSemaphore.Notify();
	}
}

void CRenderThread::EnqueueCommand(IRenderThreadCommand* InCommand)
{
	CommandsMutex.lock();
	Commands.push(InCommand);
	CommandsMutex.unlock();
}

} /* namespace ZE */