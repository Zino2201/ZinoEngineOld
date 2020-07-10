#pragma once

#include "EngineCore.h"
#include "RenderPass/RenderPassRenderer.h"
#include "MeshRendering/MeshDrawcall.h"
#include "NonCopyable.h"

namespace ZE::Renderer
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

	void AddProxy(const TOwnerPtr<CRenderableComponentProxy>& InProxy);
	void RemoveProxy(CRenderableComponentProxy* InProxy);

	auto& GetProxies() { return Proxies; }
	auto& GetCachedDrawcalls(const ERenderPass& InRenderPass) { return CachedDrawcallMap[InRenderPass]; }
private:
	std::vector<std::unique_ptr<CRenderableComponentProxy>> Proxies;
	std::unordered_map<ERenderPass, TDrawcallList> CachedDrawcallMap;
};

}