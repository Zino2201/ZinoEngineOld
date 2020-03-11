#pragma once

#include "Render/RenderCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/Renderer/Mesh/MeshPassEnum.h"

class CMaterialRenderData;

/**
 * An instance of a mesh inside a mesh collection
 * Contains it's own bindings
 */
struct SMeshCollectionInstance
{
    /** If instancing is disabled, each instance have one proxy */
    CRenderableComponentProxy* Proxy;
    // TODO: Remove

    /** UBO used by mesh passes for instance data, 
     * when instancing enable contains UBO per instance data array */
    // TODO: Map per pass ?
    IRenderSystemUniformBuffer* UniformBuffer;

    /** Instancing properties */
    uint32_t InstanceCount;

    SMeshCollectionInstance() : SMeshCollectionInstance(nullptr, nullptr) {}

    SMeshCollectionInstance(CRenderableComponentProxy* InProxy,
        IRenderSystemUniformBuffer* InUniformBuffer) : Proxy(InProxy),
        InstanceCount(1), UniformBuffer(InUniformBuffer) {}

	bool operator<(const SMeshCollectionInstance& InOther) const
	{
		return Proxy < InOther.Proxy;
	}

    bool operator==(const SMeshCollectionInstance& InOther) const
    {
        return UniformBuffer == InOther.UniformBuffer &&
            InstanceCount == InOther.InstanceCount;
    }

    operator bool() const
    {
        return UniformBuffer;
    }
};

/**
 * Static mesh data about a proxy
 * Gived by proxy's GetStaticMeshData()
 */
struct SProxyStaticMeshData
{
    CMaterialRenderData* Material;
    IRenderSystemVertexBuffer* VertexBuffer;
    IRenderSystemIndexBuffer* IndexBuffer;
    uint32_t IndexCount;
    EIndexFormat IndexFormat;
    EMeshPassFlags MeshPasses;

    SProxyStaticMeshData() : 
        SProxyStaticMeshData(nullptr, nullptr, nullptr, 0, EIndexFormat::Uint16,
            EMeshPass::GeometryPass) {}

    SProxyStaticMeshData(CMaterialRenderData* InMaterial,
		IRenderSystemVertexBuffer* InVertexBuffer,
	    IRenderSystemIndexBuffer* InIndexBuffer,
	    const uint32_t& InIndexCount,
	    EIndexFormat InIndexFormat,
	    EMeshPassFlags InMeshPasses) : Material(InMaterial),
            VertexBuffer(InVertexBuffer), IndexBuffer(InIndexBuffer), IndexCount(InIndexCount),
            IndexFormat(InIndexFormat),
            MeshPasses(InMeshPasses) {}

    operator bool() const
    {
        return Material && VertexBuffer && IndexBuffer;
    }
};

/**
 * Mesh collection
 * Contains a collection of instance of the same mesh type (material and buffers)
 */
class CMeshCollection
{
    friend class CRenderableComponentProxy;

public:
    CMeshCollection(const SProxyStaticMeshData& InStaticData) :
        CMeshCollection(InStaticData.Material,
            InStaticData.VertexBuffer,
            InStaticData.IndexBuffer,
            InStaticData.IndexCount,
            InStaticData.MeshPasses,
            true) {}

    CMeshCollection(CMaterialRenderData* InMaterial,
        IRenderSystemVertexBuffer* InVertexBuffer,
        IRenderSystemIndexBuffer* InIndexBuffer,
        const uint32_t& InIndexCount,
        EMeshPassFlags InMeshPassFlags,
        const bool& bInCanCache) :
        Material(InMaterial), VertexBuffer(InVertexBuffer), IndexBuffer(InIndexBuffer),
        IndexCount(InIndexCount), PassFlags(InMeshPassFlags),
        OptimalIndexFormat(InIndexCount > std::numeric_limits<uint16_t>::max() ? 
            EIndexFormat::Uint32 : EIndexFormat::Uint16),
        bCanCache(bInCanCache)
    {
        Instances.resize(1);
    }

    void AddInstance(const SMeshCollectionInstance& InInstance);
    void RemoveInstance(CRenderableComponentProxy* InProxy);

    bool operator==(const CMeshCollection& InOther) const
    {
        return (VertexBuffer.get() == InOther.VertexBuffer.get())
            && (Material == InOther.Material)
            && (IndexBuffer.get() == InOther.IndexBuffer.get())
            && (bCanCache == InOther.bCanCache);
    }

    bool HasDrawcalls() const
    {
        return !Instances.empty();
    }

    bool Matches(const SProxyStaticMeshData& InData) const
    {
        return InData.VertexBuffer == VertexBuffer &&
            InData.IndexBuffer == IndexBuffer &&
            InData.Material == Material;
    }

    CMaterialRenderData* GetMaterial() const { return Material; }
    IRenderSystemVertexBuffer* GetVertexBuffer() const { return VertexBuffer.get(); }
    IRenderSystemIndexBuffer* GetIndexBuffer() const { return IndexBuffer.get(); }
    const std::vector<SMeshCollectionInstance>& GetInstances() const { return Instances; }
    uint32_t GetIndexCount() const { return IndexCount; }
    EMeshPassFlags GetMeshPassFlags() const { return PassFlags; }
    EIndexFormat GetOptimalIndexFormat() const { return OptimalIndexFormat; }
protected:
    std::vector<SMeshCollectionInstance> Instances;
    CMaterialRenderData* Material;
    IRenderSystemVertexBufferPtr VertexBuffer;
    IRenderSystemIndexBufferPtr IndexBuffer;
    uint32_t IndexCount;
    EMeshPassFlags PassFlags;
    EIndexFormat OptimalIndexFormat;
    bool bCanCache;
};