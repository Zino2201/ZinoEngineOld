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

	size_t CommandsNotifyCounter = 0;
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

		/** If there is waiters for more than 100 render-frames, notify */
		if(CommandsExecutedSemaphore.HasWaiter())
		{
			CommandsNotifyCounter++;

			if(CommandsNotifyCounter > 100)
			{
				CommandsExecutedSemaphore.Notify();
				CommandsNotifyCounter = 0;
			}
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