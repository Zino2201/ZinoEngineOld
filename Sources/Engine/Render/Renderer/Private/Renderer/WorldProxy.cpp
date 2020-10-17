#include "Renderer/WorldProxy.h"
#include "Renderer/RenderableComponentProxy.h"

namespace ze::renderer
{

CWorldProxy::~CWorldProxy() = default;

void CWorldProxy::AddProxy(const OwnerPtr<CRenderableComponentProxy>& InProxy)
{
	ZE_CHECK(InProxy);

	Proxies.emplace_back(InProxy);

	/**
	 * Check if we can cache this proxy draw calls
	 */
	if(InProxy->GetType() == ERenderableComponentProxyType::Static)
	{

	}
}

void CWorldProxy::RemoveProxy(CRenderableComponentProxy* InProxy)
{
	ZE_CHECK(InProxy);

	size_t Idx = 0;
	for (const auto& Proxy : Proxies)
	{
		if (Proxy.get() == InProxy)
		{
			Proxies.erase(Proxies.begin() + Idx);
			break;
		}

		Idx++;
	}
}

}