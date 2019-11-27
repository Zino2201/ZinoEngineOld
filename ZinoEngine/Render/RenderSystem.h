#pragma once

#include "RenderCore.h"

class IRenderCommandContext;
class IShader;
class IGraphicsPipeline;

/**
 * Render system interface
 */
class IRenderSystem
{
public:
	virtual ~IRenderSystem() = default;

	/**
	 * Initialize render system
	 */
	virtual void Initialize() = 0;

	/**
	 * Prepare frame
	 */
	virtual void Prepare() = 0;

	/**
	 * Present image
	 */
	virtual void Present() = 0;

	/**
	 * Prepare destroy (wait until GPU complete all operations before deleting objects)
	 */
	virtual void PrepareDestroy() = 0;

	/**
	 * Get command context
	 */
	virtual IRenderCommandContext* GetRenderCommandContext() const = 0;

	/**
	 * Create a shader
	 */
	virtual std::shared_ptr<IShader> CreateShader(const std::vector<uint8_t>& InData,
		const EShaderStage& InShaderStage) = 0;

	/**
	 * Create a graphics pipeline
	 */
	virtual std::shared_ptr<IGraphicsPipeline> CreateGraphicsPipeline(IShader* InVertexShader,
		IShader* InFragmentShader) = 0;
};