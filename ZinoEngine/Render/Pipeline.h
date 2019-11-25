#pragma once

class IShader;

/**
 * Pipeline interface
 */
class IPipeline
{
public:
	virtual ~IPipeline() = default;
};

/**
 * Graphics pipeline
 */
class IGraphicsPipeline : public IPipeline
{
public:
	IGraphicsPipeline(IShader* InVertexShader,
		IShader* InFragmentShader) {}
	
	virtual ~IGraphicsPipeline() = default;
};