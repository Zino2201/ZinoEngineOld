#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "NonCopyable.h"
#include <optional>

namespace ZE { class IRenderSystemContext; }

namespace ZE::Renderer
{

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

	SRenderPassTextureInfos() :
		Type(ERSTextureType::Tex2D),
        Usage(ERSTextureUsage::None),
        MemoryUsage(ERSMemoryUsage::DeviceLocal),
        Format(EFormat::R8G8B8A8UNorm), Width(1), Height(1), Depth(1), ArraySize(1),
        MipLevels(1), SampleCount(ESampleCount::Sample1) {}
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

struct SRenderPassResource
{
    uint32_t ID;
    ERenderPassResourceType Type;
    SRenderPassTextureInfos TextureInfos;
    ERSRenderPassAttachmentLayout Layout;
    bool bIsRetained;

    SRenderPassResource() : ID(-1),
        Layout(ERSRenderPassAttachmentLayout::Undefined), bIsRetained(false) {}
    
    SRenderPassResource(const uint32_t& InID, const ERenderPassResourceType& InType) :
        ID(InID), Type(InType),
        Layout(ERSRenderPassAttachmentLayout::Undefined), bIsRetained(false) {}

    bool operator==(const SRenderPassResource& InOther) const
    {
        return ID == InOther.ID;
    }
};

struct SRenderPassWriteInfos
{
    const SRenderPassResource& Target;
    ERSRenderPassAttachmentLayout FinalLayout;

    SRenderPassWriteInfos(const SRenderPassResource& InResource,
        ERSRenderPassAttachmentLayout InFinalLayout) : Target(InResource), 
        FinalLayout(InFinalLayout) {}
};

struct SRenderPassReadInfos
{
	const SRenderPassResource& Target;

	SRenderPassReadInfos(const SRenderPassResource& InResource) : Target(InResource) {}
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

    SRenderPassResource& CreateTexture(const SRenderPassTextureInfos& InInfos);
    SRenderPassResource& CreateRetainedTexture(CRSTexture* InTexture);

    void Read(const SRenderPassResource& InResource);
    void Write(const SRenderPassResource& InResource, 
        ERSRenderPassAttachmentLayout InFinalLayout = ERSRenderPassAttachmentLayout::ColorAttachment);

    bool IsInput(const SRenderPassResource& InResource) const;
    bool IsOutput(const SRenderPassResource& InResource) const;

    std::optional<SRenderPassWriteInfos> GetWriteInfos(const SRenderPassResource& InResource) const;
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
    std::unordered_map<uint32_t, SEntry> TextureMap;
};

/**
 * Render pass class for a lambda-based render pass
 */
template<typename T>
class CRenderPassLambda final : public CRenderPass
{
    friend class CFrameGraph;

    using SetupFunc = std::function<void(CRenderPass& InRenderPass, T& InData)>;
    using ExecFunc = std::function<void(IRenderSystemContext* InContext, const T& InData)>;

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