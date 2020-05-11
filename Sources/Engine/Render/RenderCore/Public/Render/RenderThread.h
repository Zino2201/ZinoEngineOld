#pragma once

#include "EngineCore.h"
#include <mutex>
#include <queue>
#include "NonCopyable.h"

namespace ZE
{

/**
 * Render thread command interface
 */
class RENDERCORE_API IRenderThreadCommand
{
public:
    virtual void Execute() = 0;

    virtual const char* GetName() const = 0;
};

/**
 * Render command storing a lambda
 */
template<typename Lambda>
class CRenderThreadCommandLambda : public IRenderThreadCommand
{
public:
    CRenderThreadCommandLambda(const char* InName, Lambda&& InLambda)
        : Name(InName), Func(InLambda) {}

    virtual void Execute() override
    {
        Func();
    }

    virtual const char* GetName() const override
    {
        return Name;
    }
private:
    const char* Name;
    Lambda Func;
};

/**
 * Render class thread
 */
class CRenderThread : public CNonCopyable
{
public:
    RENDERCORE_API static CRenderThread& Get()
    {
        static CRenderThread Instance;
        return Instance;
    }

    RENDERCORE_API void Run();

    /**
     * Enqueue a command to the render thread
     */
	RENDERCORE_API void EnqueueCommand(IRenderThreadCommand* InCommand);

    bool HasCommands() const { return !Commands.empty(); }
private:
    RENDERCORE_API CRenderThread();
public:
    std::atomic_bool bRun;
    CSemaphore CommandsExecutedSemaphore;
    CSemaphore RenderThreadFrameFinishedSemaphore;
private:
    /** Commands queue */
    std::queue<IRenderThreadCommand*> Commands;

    /** Mutex to protect commands queue when render thread is executing them */
    std::mutex CommandsMutex;
};

/** Helper macro to enqueue render command */
template<typename Lambda>
FORCEINLINE void EnqueueRenderCommand(const char* InName, Lambda&& InLambda)
{
    CRenderThread::Get().EnqueueCommand(new CRenderThreadCommandLambda(InName, 
        std::forward<Lambda>(InLambda)));
}

/**
 * Wait for all render thread commands to be executed
 * If bInWaitForFrame == true, wait for the render thread frame to be completed
 */
FORCEINLINE void FlushRenderThread(const bool& bInWaitForFrame = false)
{
	while (CRenderThread::Get().HasCommands())
    {
		if (bInWaitForFrame)
			CRenderThread::Get().RenderThreadFrameFinishedSemaphore.Wait();
		else
		    CRenderThread::Get().CommandsExecutedSemaphore.Wait();
    }
}

}