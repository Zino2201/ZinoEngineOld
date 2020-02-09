#pragma once

#include "Core/EngineCore.h"

class CRenderSystemSampler;
class CRenderSystemTexture;
class CRenderSystemTextureView;
class CMaterial;

/**
 * ImGui integration
 */
class CImGui
{
public:
    CImGui();
    ~CImGui();
private:
    std::shared_ptr<CRenderSystemSampler> Sampler;
    std::shared_ptr<CRenderSystemTexture> Texture;
    std::shared_ptr<CRenderSystemTextureView> TextureView;
    std::shared_ptr<CMaterial> UIMaterial;
};