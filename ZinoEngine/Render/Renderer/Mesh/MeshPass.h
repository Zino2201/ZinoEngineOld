#pragma once

#include "Render/RenderCore.h"
#include "MeshCollection.h"
#include "MeshPassEnum.h"
#include "Render/RenderSystem/RenderSystemResources.h"

class IRenderCommandContext;
class CScene;

struct SMeshPassViewData
{
	alignas(16) glm::mat4 ProjView;
};

/**
 * Represents a shader binding for a draw command
 */
struct SMeshPassDrawCommandShaderBinding
{
    uint32_t Set;
    uint32_t Binding;
    union
    {
        IRenderSystemUniformBuffer* UniformBuffer;
        CRenderSystemTextureView* TextureView;
        CRenderSystemBuffer* StorageBuffer;
    };
    EShaderParameterType Type;

    SMeshPassDrawCommandShaderBinding() : Set(0), Binding(0), UniformBuffer(nullptr),
        TextureView(nullptr), StorageBuffer(nullptr), Type(EShaderParameterType::UniformBuffer) {}

	SMeshPassDrawCommandShaderBinding(const uint32_t& InSet,
        const uint32_t& InBinding, IRenderSystemUniformBuffer* InUBO) : 
        Set(InSet), Binding(InBinding), UniformBuffer(InUBO), Type(EShaderParameterType::UniformBuffer) {}

	SMeshPassDrawCommandShaderBinding(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemTextureView* InView) :
		Set(InSet), Binding(InBinding), TextureView(InView), Type(EShaderParameterType::CombinedImageSampler) {}

	SMeshPassDrawCommandShaderBinding(const uint32_t& InSet,
		const uint32_t& InBinding, CRenderSystemBuffer* InStorageBuffer) :
		Set(InSet), Binding(InBinding), StorageBuffer(InStorageBuffer), Type(EShaderParameterType::StorageBuffer) {}
};

namespace std
{
	template<> struct hash<SMeshPassDrawCommandShaderBinding>
	{
		size_t operator()(const SMeshPassDrawCommandShaderBinding& InBinding) const
		{
			return hash<uint32_t>()(InBinding.Set) ^
				hash<uint32_t>()(InBinding.Binding);
		}
	};
}

/**
 * A mesh draw call for a single mesh pass
 * Contains everything required to draw a mesh collection
 */
class CMeshPassDrawCommand
{
    friend class CMeshPass;

public:
    CMeshPassDrawCommand() : Pipeline(nullptr),
        VertexBuffer(nullptr),
        IndexBuffer(nullptr),
        IndexCount(0),
        UniformBuffer(nullptr) {}

	CMeshPassDrawCommand(IRenderSystemGraphicsPipeline* InPipeline,
		IRenderSystemVertexBuffer* InVertexBuffer,
		IRenderSystemIndexBuffer* InIndexBuffer,
		const uint32_t& InIndexCount,
        IRenderSystemUniformBuffer* InUniformBuffer);

    void AddShaderBinding(const SMeshPassDrawCommandShaderBinding& InBinding)
    {
        Bindings.Add(InBinding);
    }

    void AddInstance(CRenderableComponentProxy* InProxy);
    void UpdateInstancesData();

    void MergeWith(const CMeshPassDrawCommand& InDrawCommand);
    void Execute(IRenderCommandContext* InContext) const;

    bool operator==(const CMeshPassDrawCommand& InOther) const
    {
        return Pipeline == InOther.Pipeline &&
            VertexBuffer == InOther.VertexBuffer &&
            IndexBuffer == InOther.IndexBuffer &&
            IndexCount == InOther.IndexCount &&
            IndexFormat == InOther.IndexFormat &&
            InstanceCount == InOther.InstanceCount;
    }

    IRenderSystemGraphicsPipeline* GetPipeline() const { return Pipeline; }
    IRenderSystemVertexBuffer* GetVertexBuffer() const { return VertexBuffer; }
    IRenderSystemIndexBuffer* GetIndexBuffer() const { return IndexBuffer; }
    IRenderSystemUniformBuffer* GetUniformBuffer() const { return UniformBuffer; }
    uint32_t GetIndexCount() const { return IndexCount; }
private:
    IRenderSystemGraphicsPipeline* Pipeline;
    IRenderSystemVertexBuffer* VertexBuffer;
    IRenderSystemIndexBuffer* IndexBuffer;
    IRenderSystemUniformBuffer* UniformBuffer;
    uint32_t IndexCount;
    EIndexFormat IndexFormat;
    uint32_t InstanceCount;
    IRenderSystemVertexBufferPtr InstanceBuffer;
    uint64_t InstanceBufferCount;
    uint64_t InstanceBufferCapacity;
    TSet<CRenderableComponentProxy*> InstanceProxies;
    TSet<SMeshPassDrawCommandShaderBinding> Bindings;
};

namespace std
{
	template<> struct hash<CMeshPassDrawCommand>
	{
		size_t operator()(const CMeshPassDrawCommand& InCmd) const
		{
			return hash<IRenderSystemVertexBuffer*>()(InCmd.GetVertexBuffer()) ^
			    hash<IRenderSystemIndexBuffer*>()(InCmd.GetIndexBuffer()) ^
			    hash<IRenderSystemGraphicsPipeline*>()(InCmd.GetPipeline());
		}
	};
}

/**
 * Base interface for a mesh pass
 * Used to render a CMeshCollection by creating a CMeshPassDrawcall
 */
class CMeshPass
{
public:
    CMeshPass(EMeshPass InPass);

    static void DestroyMeshPasses()
    {
        if(MeshPasses)
        {
            delete MeshPasses;
            MeshPasses = nullptr;
        }
    }

    static CMeshPass* GetMeshPass(EMeshPass InPass)
    {
        auto& MeshPass = MeshPasses->find(InPass);

        if(MeshPass == MeshPasses->end())
            return nullptr;
        else
            return MeshPass->second;
    }

    /**
     * Build a draw command for that mesh pass for the specified scene
     */
    virtual void BuildDrawCommand(CScene* InScene,
        CRenderableComponentProxy* InProxy,
        const CMeshCollection& InCollection,
        const uint32_t& InMeshInstanceIdx);
private:
    static std::map<EMeshPass, CMeshPass*>* MeshPasses;
    EMeshPass Pass;
};

#define DEFINE_MESH_PASS(MeshPass, Enum) static MeshPass MeshPass(Enum)