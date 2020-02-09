#pragma once

#include "Core/Asset.h"
#include "Render/RenderSystem/RenderSystemResources.h"
#include "RenderCore.h"

class CRenderSystemBuffer;

class CStaticMesh : public IAsset
{
public:
	virtual void Load(const std::string& InPath) override;

	CRenderSystemBuffer* GetVertexBuffer() const { return VertexBuffer.get(); }
	CRenderSystemBuffer* GetIndexBuffer() const { return IndexBuffer.get(); }
	const uint32_t& GetIndexCount() const { return IndexCount; }
	EIndexFormat GetOptimalIndexFormat() const { return IndexFormat; }
private:
	CRenderSystemBufferPtr VertexBuffer;
	CRenderSystemBufferPtr IndexBuffer;
	uint32_t IndexCount;
	EIndexFormat IndexFormat;
	/** Static mesh */
};