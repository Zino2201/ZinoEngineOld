#pragma once

#include <queue>

/**
 * Base class for resources that need to be destroyed once the frame has been completed
 */
//class RENDERCORE_API IDeferredDestructionRenderResource
//{
//public:
//    /** The "fake" destroy function */
//    virtual void Destroy();
//
//    /** The actual Destroy function */
//    virtual void FinishDestroy() = 0;
//    
//    static IDeferredDestructionRenderResource* /*std::queue<IDeferredDestructionRenderResource*>*/ Resources;
//    static void DestroyResources();
//};

class IRenderSystemResource;

///** 
// * IFrameCompletedDestruction for render system resource
// * Hold a intrusive_ptr so the resource is still available
// */
//class RENDERCORE_API CRenderSystemResourceFrameCompletedDestruction 
//	: public IDeferredDestructionRenderResource
//{
//public:
//	CRenderSystemResourceFrameCompletedDestruction(IRenderSystemResource*
//		InResource) : Resource(InResource) {}
//
//	static void Initialize();
//	static void OnResourceDestroyed(IRenderSystemResource* InResource);
//	virtual void FinishDestroy() override;
//private:
//	TIntrusivePtr<IRenderSystemResource> Resource;
//};