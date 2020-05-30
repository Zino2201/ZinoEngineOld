#pragma once

#include "Renderer/FrameGraph/FrameGraph.h"

namespace ZE::Renderer
{

struct SBarrelDistortionData
{
	RenderPassResourceID Target;
};

const SBarrelDistortionData& AddBarrelDistortionPass(CFrameGraph& InFrameGraph, 
	const RenderPassResourceID& InColor,
	const RenderPassResourceID& InTarget);

}