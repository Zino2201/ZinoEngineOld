#pragma once

#include "EngineCore.h"
#include "RenderPass/RenderPassRenderer.h"
#include "MeshRendering/MeshDrawcall.h"
#include "NonCopyable.h"

namespace ze::renderer
{

class CRenderableComponentProxy;

/**
 * Renderer version of CWorld
 * Contains proxies
 */
class RENDERER_API CWorldProxy final : public CNonCopyable
{
public:
	~CWorldProxy();

	void AddProxy(const OwnerPtr<CRenderableComponentProxy>& InProxy);
	void RemoveProxy(CRenderableComponentProxy* InProxy);

	auto& GetProxies() { return Proxies; }
	auto& GetCachedDrawcalls(const ERenderPassFlagBits& InRenderPass) { return CachedDrawcallMap[InRenderPass]; }
private:
	std::vector<std::unique_ptr<CRenderableComponentProxy>> Proxies;
	robin_hood::unordered_map<ERenderPassFlagBits, TDrawcallList> CachedDrawcallMap;
};

}