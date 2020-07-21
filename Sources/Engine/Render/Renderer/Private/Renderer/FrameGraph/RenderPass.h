#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "Render/RenderSystem/Resources/Texture.h"
#include "Render/RenderSystem/Resources/RenderPass.h"
#include "NonCopyable.h"
#include <optional>
#include <robin_hood.h>

namespace ZE { class IRenderSystemContext; }

namespace ZE::Renderer
{

DECLARE_LOG_CATEGORY(FrameGraph);

class CFrameGraph;

/**
 * A render pass texture
 */
struct SRenderPassTextureInfos
{
    ERSTextureType Type;
    ERSTextureUsage Usage;
    ERSMemoryUsage MemoryUsage;
    EFormat Format;
    uint32_t Width;
    uint32_t Height;
    uint32_t Depth;
    uint32_t ArraySize;
    uint32_t MipLevels;
    ESampleCount SampleCount;
    ERSRenderPassAttachmentLoadOp LoadOp;

	SRenderPassTextureInfos() :
		Type(ERSTextureType::Tex2D),
        Usage(ERSTextureUsage::None),
        MemoryUsage(ERSMemoryUsage::DeviceLocal),
        Format(EFormat::R8G8B8A8UNorm), Width(1), Height(1), Depth(1), ArraySize(1),
        MipLevels(1), SampleCount(ESampleCount::Sample1), 
        LoadOp(ERSRenderPassAttachmentLoadOp::Clear) {}
};

struct SRenderPassTextureInfosHash
{
    uint64_t operator()(const SRenderPassTextureInfos& InInfos) const
    {
        uint64_t Hash = 0;

        HashCombine(Hash, InInfos.Width);
        HashCombine(Hash, InInfos.Height);

        return Hash;
    }
};

enum class ERenderPassResourceType
{
    Texture
};

using RenderPassResourceID = uint32_t;

struct SRenderPassResource
{
    RenderPassResourceID ID;
    ERenderPassResourceType Type;
    SRenderPassTextureInfos TextureInfos;
    ERSRenderPassAttachmentLayout Layout;
    bool bIsRetained;

    SRenderPassResource() : ID(-1),
        Layout(ERSRenderPassAttachmentLayout::Undefined), bIsRetained(false) {}
    
    SRenderPassResource(const RenderPassResourceID& InID, const ERenderPassResourceType& InType) :
        ID(InID), Type(InType),
        Layout(ERSRenderPassAttachmentLayout::Undefined), bIsRetained(false) {}

    bool operator==(const SRenderPassResource& InOther) const
    {
        return ID == InOther.ID;
    }

    bool operator==(const RenderPassResourceID& InID) const
    {
        return ID == InID;
    }
};

/**
 * Layout of the resource
 */
enum class ERenderPassResourceLayout
{
    /** Optimal for shader read */
    ShaderReadOnlyOptimal,

    /** Optimal for being written to */
    RenderTarget,

    /** Optimal for presentation */
    Present
};

struct SRenderPassWriteInfos
{
    RenderPassResourceID Target;
    ERenderPassResourceLayout FinalLayout;
    ERSRenderPassAttachmentStoreOp StoreOp;

    SRenderPassWriteInfos(const RenderPassResourceID& InResource,
        ERenderPassResourceLayout InFinalLayout,
        ERSRenderPassAttachmentStoreOp InStoreOp) : Target(InResource), 
        FinalLayout(InFinalLayout), StoreOp(InStoreOp) {}
};

struct SRenderPassReadInfos
{
	RenderPassResourceID Target;
    ERSRenderPassAttachmentLoadOp LoadOp;

	SRenderPassReadInfos(const RenderPassResourceID& InResource,
        ERSRenderPassAttachmentLoadOp InLoadOp) : Target(InResource), LoadOp(InLoadOp) {}
};

/**
 * A render-pass
 */
class CRenderPass
{
    friend class CFrameGraph;

public:
    CRenderPass(const uint32_t InID, const std::string_view& InName,
        CFrameGraph& InGraph) : ID(InID), Name(InName), Graph(InGraph) {}

    virtual void Execute(IRenderSystemContext* InContext) {}

    RenderPassResourceID& CreateTexture(const SRenderPassTextureInfos& InInfos);
    RenderPassResourceID& CreateRetainedTexture(CRSTexture* InTexture);

    void SetLoadOp(const RenderPassResourceID& InResource, ERSRenderPassAttachmentLoadOp InLoadOp);

    void Read(const RenderPassResourceID& InResource,
        ERSRenderPassAttachmentLoadOp InLoadOp = ERSRenderPassAttachmentLoadOp::Clear);
    void Write(const RenderPassResourceID& InResource, 
        ERenderPassResourceLayout InFinalLayout = ERenderPassResourceLayout::ShaderReadOnlyOptimal);

    bool IsInput(const SRenderPassResource& InResource) const;
    bool IsOutput(const SRenderPassResource& InResource) const;

    std::optional<SRenderPassWriteInfos> GetWriteInfos(const SRenderPassResource& InResource) const;
    std::optional<SRenderPassReadInfos> GetReadInfos(const SRenderPassResource& InResource) const;
private:
    SRenderPassResource CreateResource(ERenderPassResourceType InType);
protected:
    CFrameGraph& Graph;
    uint32_t ID;
    std::string Name;
    std::vector<SRenderPassResource> Textures;
    std::vector<SRenderPassWriteInfos> WriteInfosMap;
    std::vector<SRenderPassReadInfos> ReadInfos;
    std::vector<CRenderPass*> Dependencies;
    robin_hood::unordered_set<RenderPassResourceID> Attachments;
    SRSRenderPass PhysRenderPass;
};

/**
 * A singleton class that manages persistent resources
 * Also manage lifetimes when any frame graph are executed
 */

constexpr uint32_t GTextureLifetime = 5;

class CRenderPassPersistentResourceManager final : public CNonCopyable
{
    struct SEntry
    {
        CRSTexturePtr Texture;
        SRenderPassTextureInfos Infos;
        uint32_t InactiveCounter;

        SEntry() : Texture(nullptr), Infos(), InactiveCounter(0) {}
    };

public:
    static CRenderPassPersistentResourceManager& Get()
    {
        static CRenderPassPersistentResourceManager Instance;
        return Instance;
    }

    CRSTexture* GetOrCreateTexture(const uint32_t& InID, const SRenderPassTextureInfos& InInfos);
    void UpdateLifetimes();
    void Destroy() { TextureMap.clear(); }
private:
    robin_hood::unordered_map<uint32_t, SEntry> TextureMap;
};

/**
 * Render pass class for a lambda-based render pass
 */
template<typename T>
using RenderPassLambdaSetupFunc = void(*)(CRenderPass& InRenderPass, T& InData);
template<typename T>
using RenderPassLambdaExecFunc = void(*)(IRenderSystemContext* InContext, const T& InData);

template<typename T, typename SetupFunc, typename ExecFunc>
class CRenderPassLambda final : public CRenderPass
{
    friend class CFrameGraph;

public:
    CRenderPassLambda(const uint32_t& InID,
        const std::string_view& InName,
        CFrameGraph& InFrameGraph,
		const SetupFunc& InSetupFunc,
		const ExecFunc& InExecutionFunc) : CRenderPass(InID, InName, InFrameGraph),
        Setup(InSetupFunc), Exec(InExecutionFunc) 
    {
        Setup(*this, Data);
    }

    void Execute(IRenderSystemContext* InContext) override
    {
        Exec(InContext, Data);
    }
private:
    T Data;
    SetupFunc Setup;
    ExecFunc Exec;
};

}