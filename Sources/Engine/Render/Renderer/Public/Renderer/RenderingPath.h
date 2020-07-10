#pragma once

namespace ZE::Renderer
{

class CWorldRenderer;
struct SWorldView;

/**
 * Interface for rendering paths that control how the world is rendered using
 * renderer's data
 */
class IRenderingPath
{
public:
	virtual void Draw(const SWorldView& InView) = 0;
};

}