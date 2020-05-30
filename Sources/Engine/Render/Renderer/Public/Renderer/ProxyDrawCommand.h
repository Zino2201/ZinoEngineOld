#pragma once

#include "EngineCore.h"
#include "Render/RenderSystem/RenderSystemResources.h"

namespace ZE::Renderer
{

/**
 * A shader binding of a draw command
 */
struct SProxyDrawCommandShaderBinding
{
    uint32_t Set;
    uint32_t Binding;
    CRSBufferPtr Buffer;

    SProxyDrawCommandShaderBinding() : Set(0), Binding(0), Buffer(nullptr) {}
    SProxyDrawCommandShaderBinding(const uint32_t& InSet,
        const uint32_t& InBinding,
        CRSBuffer* InBuffer) : Set(InSet), Binding(InBinding), Buffer(InBuffer) {}
};

/**
 * A cached draw command
 */
class CProxyDrawCommand
{
public:
    CProxyDrawCommand() : Pipeline(nullptr), VertexBuffer(nullptr),
        IndexBuffer(nullptr), bDrawIndexed(false), IndexFormat(EIndexFormat::Uint16),
        IndexCount(0), VertexCount(0) {}

	CProxyDrawCommand(CRSGraphicsPipeline* InPipeline,
		CRSBuffer* InVertexBuffer,
		CRSBuffer* InIndexBuffer,
		const uint32_t& InIndexCount) : Pipeline(InPipeline),
        VertexBuffer(InVertexBuffer), IndexBuffer(InIndexBuffer),
		IndexCount(InIndexCount), IndexFormat(InIndexCount > std::numeric_limits<uint16_t>::max() ?
			EIndexFormat::Uint32 : EIndexFormat::Uint16) {}

	void AddBinding(const uint32_t& InSet,
		const uint32_t& InBinding,
		CRSBuffer* InBuffer);

    bool operator==(const CProxyDrawCommand& InDrawCommand) const
    {
        return Pipeline == InDrawCommand.Pipeline;
    }

    uint32_t GetIndexCount() const { return IndexCount; }
    EIndexFormat GetIndexFormat() const { return IndexFormat; }
    const std::vector<SProxyDrawCommandShaderBinding>& GetBindings() const { return Bindings; }
private:
    CRSGraphicsPipelinePtr Pipeline;
    CRSBufferPtr VertexBuffer;
    CRSBufferPtr IndexBuffer;
    bool bDrawIndexed;
    EIndexFormat IndexFormat;
    uint32_t IndexCount;
    uint32_t VertexCount;
    uint32_t InstanceCount;
    std::vector<SProxyDrawCommandShaderBinding> Bindings;
};

struct SProxyDrawCommandHash
{
    uint64_t operator()(const CProxyDrawCommand& InCommand) const
    {
        uint64_t Hash = 0;
        static int a = 0;
        a++;
        Hash += a;
        return Hash;
    }
};

}