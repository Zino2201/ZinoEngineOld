#pragma once

#include "EngineCore.h"
#include "Render/Commands/Commands.h"
#include <queue>

class CRenderableComponent;
class CRenderCommandList;
class CRenderableComponentProxy;
class IRenderCommandContext;
class ISceneRenderer;

/**
 * Base class for render thread resources
 * Init render system resources in InitRenderThread
 */
class CRenderResource
{
public:
    virtual ~CRenderResource();

    /**
     * Init resources function (enqueue to render thread)
     */
    virtual void InitResources();

    /**
     * Destroy resources
     */
    virtual void DestroyResources();

    CSemaphore& GetDestroyedSemaphore() { return DestroyedSemaphore; }
protected:
    /** Init the resource */
    virtual void InitRenderThread() {}

    /** Destroy the resource */
    virtual void DestroyRenderThread() {}
private:
    bool bInitialized;
    CSemaphore DestroyedSemaphore;
};

/**
 * Base class for resources that need to be destroyed once the frame has been completed
 */
class IDeferredDestructionRenderResource
{
public:
    /** The "fake" destroy function */
    virtual void Destroy();

    /** The actual Destroy function */
    virtual void FinishDestroy() = 0;
    
    static std::queue<IDeferredDestructionRenderResource*> Resources;
    static void DestroyResources();
};

/**
 * The render thread singleton
 */
class CRenderThread
{
public:
	static CRenderThread& Get()
	{
		static CRenderThread Instance;
		return Instance;
	}

    void Initialize();
    void Start();
    void Stop();

    /** Destroy renderer */
    void DestroyRenderer();

    CSemaphore& GetRenderThreadSemaphore() { return RenderThreadSemaphore; }
    CRenderCommandList* GetRenderCommandList() const { return RenderThreadCommandList; }
private:
    void RenderThreadMain();
public:
	CRenderThread(const CRenderThread&) = delete;
	void operator=(const CRenderThread&) = delete;

    /**
     * Should render thread render
     */
    std::atomic_bool ShouldRender;
    std::atomic_bool ShouldWaitGT;
private:
	CRenderThread();
	~CRenderThread();
private:
    /**
     * Should render thread loop
     */
    std::atomic_bool LoopRenderThread;

    /**
     * Render thread handle
     */
    std::thread RenderThreadHandle;

	/**
	 * Semaphore used to wait the render thread
	 */
	CSemaphore RenderThreadSemaphore;

    /**
     * Render thread command list
     */
    CRenderCommandList* RenderThreadCommandList;

    /**
     * Scene renderer
     */
    ISceneRenderer* SceneRenderer;
};

/**
 * Enqueue render command function alias
 */
template<typename Lambda>
FORCEINLINE void EnqueueRenderCommand(Lambda&& InLambda)
{
    CRenderThread::Get().GetRenderCommandList()->Enqueue(std::forward<Lambda>(InLambda));
}