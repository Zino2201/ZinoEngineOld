#include "Renderer/ProxyDrawCommand.h"

namespace ZE::Renderer
{

void CProxyDrawCommand::AddBinding(const uint32_t& InSet,
	const uint32_t& InBinding,
	CRSBuffer* InBuffer)
{
	Bindings.emplace_back(InSet, InBinding, InBuffer);
}

}