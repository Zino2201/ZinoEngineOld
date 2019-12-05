#pragma once

#include "Core/Asset.h"

class IBuffer;

class CStaticMesh : public IAsset
{
public:
	virtual void Load(const std::string& InPath) override;

	const std::shared_ptr<IBuffer>& GetVertexBuffer() const { return VertexBuffer; }
	const std::shared_ptr<IBuffer>& GetIndexBuffer() const { return IndexBuffer; }
	const uint32_t& GetIndexCount() const { return IndexCount; }
private:
	std::shared_ptr<IBuffer> VertexBuffer;
	std::shared_ptr<IBuffer> IndexBuffer;
	uint32_t IndexCount;
};