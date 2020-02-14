#include "Core/Engine.h"
#include "Core/RenderThread.h"

int main()
{
	/** Initialize and run engine */
	g_Engine = new CEngine;
	g_Engine->Initialize();
	delete g_Engine;

	/** Stop game thread at end */
	CRenderThread::Get().Stop();

	return 0;
}