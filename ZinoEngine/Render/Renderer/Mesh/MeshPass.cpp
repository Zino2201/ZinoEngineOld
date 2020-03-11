#include "MeshPass.h"
#include "Render/Commands/RenderCommandContext.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Material/Material.h"
#include "Render/World/RenderableComponentProxy.h"
#include "Render/Renderer/Scene.h"
#include "World/World.h"
#include "Render/Commands/RenderCommandContext.h"
#include "World/Components/StaticMeshComponent.h"
#include "Core/Engine.h"
#include "Render/Window.h"

DECLARE_COUNTER_STAT_EXTERN(Drawcall);

std::map<EMeshPass, CMeshPass*>* CMeshPass::MeshPasses;

CMeshPass::CMeshPass(EMeshPass InPass) : Pass(InPass)
{
	if(!MeshPasses)
		MeshPasses = new std::map<EMeshPass, CMeshPass*>;

	MeshPasses->insert(std::make_pair(InPass, this));
}

void CMeshPass::BuildDrawCommand(CScene* InScene,
	CRenderableComponentProxy* InProxy,
	const CMeshCollection& InCollection,
	const uint32_t& InMeshInstanceIdx)
{
	/** Create a mesh draw command for the specified collection's instance */
	if(InProxy)
	{
		CMeshPassDrawCommand DrawCommand(InCollection.GetMaterial()->GetPipeline(),
			InCollection.GetVertexBuffer(),
			InCollection.GetIndexBuffer(),
			InCollection.GetIndexCount(),
			InCollection.GetInstances()[InMeshInstanceIdx].UniformBuffer);

		DrawCommand.AddInstance(InProxy);

		/** Setup shader bindings */
		glm::vec3 LightPos = glm::vec3(0, 5, 10);
		memcpy(InProxy->GetLightUBO()->GetMappedMemory(), &LightPos, sizeof(LightPos));

		/** Material bindings */
		CMaterialRenderData* Material = InCollection.GetMaterial();
		DrawCommand.AddShaderBinding(SMeshPassDrawCommandShaderBinding(
			Material->GetScalars().GetShaderParameter().Set,
			Material->GetScalars().GetShaderParameter().Binding,
			Material->GetScalars().GetUniformBuffer()));
		DrawCommand.AddShaderBinding(SMeshPassDrawCommandShaderBinding(
			Material->GetVec3s().GetShaderParameter().Set,
			Material->GetVec3s().GetShaderParameter().Binding,
			Material->GetVec3s().GetUniformBuffer()));

		/** Instance bindings */
		DrawCommand.AddShaderBinding(SMeshPassDrawCommandShaderBinding(
			2,
			1,
			InProxy->GetLightUBO()));

		SSetElementId DrawCommandId = InScene->GetDrawCommandListManager().AddCommand(Pass,
			DrawCommand);

		SProxyCachedCommand CachedCommand = { Pass, DrawCommandId };
		InProxy->DrawCommands.push_back(CachedCommand);
	}
}

CMeshPassDrawCommand::CMeshPassDrawCommand(IRenderSystemGraphicsPipeline* InPipeline,
	IRenderSystemVertexBuffer* InVertexBuffer,
	IRenderSystemIndexBuffer* InIndexBuffer,
	const uint32_t& InIndexCount,
	IRenderSystemUniformBuffer* InUniformBuffer)
{
	Pipeline = InPipeline;
	VertexBuffer = InVertexBuffer;
	IndexBuffer = InIndexBuffer;
	IndexCount = InIndexCount;
	IndexFormat = InIndexCount > std::numeric_limits<uint16_t>::max() ?
		EIndexFormat::Uint32 : EIndexFormat::Uint16;
	UniformBuffer = InUniformBuffer;
	InstanceBuffer = nullptr;
}

void CMeshPassDrawCommand::MergeWith(const CMeshPassDrawCommand& InDrawCommand)
{
	for(uint64_t i = 0; i < InDrawCommand.InstanceProxies.GetCount(); ++i)
	{
		AddInstance(InDrawCommand.InstanceProxies[i]);
	}
}

void CMeshPassDrawCommand::AddInstance(CRenderableComponentProxy* InProxy)
{
	if(InstanceBuffer)
		InstanceBuffer->Destroy();

	InstanceBuffer = g_Engine->GetRenderSystem()->CreateVertexBuffer(
		InstanceBufferCapacity + sizeof(uint32_t),
		EBufferMemoryUsage::CpuToGpu,
		false,
		"InstanceBuffer");

	InstanceBufferCapacity += sizeof(uint32_t);

	InstanceProxies.Add(InProxy);
	
	InstanceBufferCount++;	
	InstanceCount++;

	UpdateInstancesData();
}

void CMeshPassDrawCommand::UpdateInstancesData()
{
	/** Update instancing datas */
	uint8_t* Dst = reinterpret_cast<uint8_t*>(InstanceBuffer->Map());

	for(uint64_t i = 0; i < InstanceProxies.GetCount(); ++i)
	{
		CRenderableComponentProxy* Proxy = InstanceProxies[i];

		memcpy(Dst + i * sizeof(uint32_t),
			&Proxy->RenderableDataIdx, sizeof(uint32_t));
	}

	InstanceBuffer->Unmap();
}

void CMeshPassDrawCommand::Execute(IRenderCommandContext* InContext) const
{
	if(!Pipeline || !VertexBuffer || !IndexBuffer || !UniformBuffer)
		return;

	INCREMENT_STAT(Drawcall);

	InContext->BindGraphicsPipeline(Pipeline);

	for(uint64_t i = 0; i < Bindings.GetCount(); ++i)
	{
		const SMeshPassDrawCommandShaderBinding& Binding = Bindings[i];

		switch(Binding.Type)
		{
		case EShaderParameterType::UniformBuffer:
			InContext->SetShaderUniformBuffer(Binding.Set, Binding.Binding,
				Binding.UniformBuffer);
			break;
		case EShaderParameterType::StorageBuffer:
			InContext->SetShaderStorageBuffer(Binding.Set, Binding.Binding,
				Binding.StorageBuffer);
			break;
		case EShaderParameterType::CombinedImageSampler:
			InContext->SetShaderCombinedImageSampler(Binding.Set, Binding.Binding,
				Binding.TextureView);
			break;
		}
	}

	/** Bind scene ssbo */
	InContext->SetShaderStorageBuffer(
		2,
		0,
		g_Engine->GetWorld()->GetScene()->GetRenderableProxyDataManager()->GetBuffer());

	InContext->BindVertexBuffers({ VertexBuffer, InstanceBuffer.get() });

	InContext->BindIndexBuffer(IndexBuffer,
		0,
		IndexFormat);

	InContext->DrawIndexed(IndexCount, InstanceCount, 0, 0, 0);
}