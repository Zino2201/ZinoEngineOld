#include "Renderer/RenderPass/RenderPassRenderer.h"
#include "Renderer/MeshRendering/RenderPassDrawcallFactory.h"
#include "Renderer/WorldView.h"
#include "Renderer/WorldProxy.h"
#include "Threading/JobSystem/ParallelFor.h"
#include "Render/RenderSystem/RenderSystemContext.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ZE::Renderer
{

CRenderPassRenderer::CRenderPassRenderer(CWorldProxy& InProxy) : WorldProxy(InProxy) {}

void CRenderPassRenderer::PrepareDrawcalls(SWorldView& InView, 
	const ERenderPassFlagBits& InRenderPass,
	CRenderPassDrawcallFactory& InDrawcallFactory)
{
	using namespace JobSystem;

	InDrawcallFactory.SetDrawcallList(&DynamicDrawcalls);

	/**
	 * Prepare data
	 */
	DynamicDrawcalls.Reserve(InView.VisibleDynamicMeshes.size());

	if(InRenderPass != ERenderPassFlagBits::None)
		FinalDrawcalls.reserve(
			DynamicDrawcalls.GetCapacity() + WorldProxy.GetCachedDrawcalls(InRenderPass).GetSize());
	else
		FinalDrawcalls.reserve(DynamicDrawcalls.GetCapacity());

	/**
	 * Create and sort dynamic drawcalls
	 */
	PrepareJob = &CreateJob(EJobType::Normal,
		[&, InRenderPass](const SJob& InJob)
		{
			/**
			 * Generate drawcalls for dynamic mesh batches
			 */
			for(size_t i = 0; i < InView.VisibleDynamicMeshes.size(); ++i)
			{
				if(InRenderPass != ERenderPassFlagBits::None 
					&& InView.VisibleDynamicMeshRenderPassFlags[i] & InRenderPass)
				{
					InDrawcallFactory.ProcessMesh(InView, *InView.VisibleDynamicMeshProxies[i],
						InView.VisibleDynamicMeshes[i]);
				}
			}

			/** Sort */
			for(const auto& Call : DynamicDrawcalls)
				FinalDrawcalls.emplace_back(&Call);
		});
	ScheduleJob(*PrepareJob);
}

void CRenderPassRenderer::Draw(IRenderSystemContext& InContext) const
{
	using namespace JobSystem;

	/** Wait for prepare job if not finished */
	WaitJob(*PrepareJob);

	for(const auto& Drawcall : FinalDrawcalls)
	{
		SubmitDrawcall(InContext, *Drawcall);
	}
}

void CRenderPassRenderer::SubmitDrawcall(IRenderSystemContext& InContext, 
	const SMeshDrawcall& InDrawcall) const
{
	InContext.BindGraphicsPipeline(InDrawcall.Pipeline);
	
	InContext.BindVertexBuffers({ InDrawcall.VertexBuffer });
	InContext.BindIndexBuffer(InDrawcall.IndexBuffer, 0, InDrawcall.IndexFormat);

	for(const auto& Binding : InDrawcall.Bindings)
	{
		if(!Binding.Buffer)
			continue;

		switch(Binding.ParameterType)
		{
		default:
			ZE::Logger::Error("Unknown parameter type for binding {} set {}",
				Binding.Binding, Binding.Set);
			break;
		case EShaderParameterType::UniformBuffer:
			InContext.SetShaderUniformBuffer(Binding.Set, Binding.Binding, Binding.Buffer);
			break;
		case EShaderParameterType::Texture:
			InContext.SetShaderTexture(Binding.Set, Binding.Binding, Binding.Texture);
			break;
		case EShaderParameterType::Sampler:
			InContext.SetShaderSampler(Binding.Set, Binding.Binding, Binding.Sampler);
			break;
		}
	}

	if(!InDrawcall.IndexBuffer)
		InContext.Draw(InDrawcall.VertexCount, InDrawcall.InstanceCount,
			InDrawcall.FirstIndex, InDrawcall.FirstInstance);
	else
		InContext.DrawIndexed(InDrawcall.IndexCount, InDrawcall.InstanceCount,
			InDrawcall.FirstIndex, 0, InDrawcall.FirstInstance);
}

}