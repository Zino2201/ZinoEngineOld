#pragma once

#include "EngineCore.h"

class CRenderableComponent;
class CRenderCommandList;
class CRenderableComponentProxy;

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
protected:
    /** Init the resource */
    virtual void InitRenderThread() {}

    /** Destroy the resource */
    virtual void DestroyRenderThread() {}
private:
    bool bInitialized;
};

/**
 * Proxies used by the render thread to draw the scene and other
 */

/**
 * Renderable component proxy
 */
class CRenderableComponentProxy
{
public:
    CRenderableComponentProxy(const CRenderableComponent* InComponent);

    virtual void Draw(CRenderCommandList* InCommandList);
    
    const std::shared_ptr<class IShaderAttributesManager>& GetShaderAttributesManager() const { return ShaderAttributesManager; }
protected:
    STransform Transform;
    std::shared_ptr<class IShaderAttributesManager> ShaderAttributesManager;
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
};

/**
 * Enqueue render command function alias
 */
template<typename Lambda>
FORCEINLINE void EnqueueRenderCommand(Lambda&& InLambda)
{
    CRenderThread::Get().GetRenderCommandList()->Enqueue(std::forward<Lambda>(InLambda));
}