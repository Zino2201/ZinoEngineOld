#include "EpilepsieShader.h"

DECLARE_SHADER_CLASS(CEpilepsieShaderVS, "EpilepsieShaderVS", "Assets/Shaders/epilepsie.vert", 
	EShaderStage::Vertex)
DECLARE_SHADER_CLASS(CEpilepsieShaderFS, "EpilepsieShaderFS", "Assets/Shaders/epilepsie.frag", 
	EShaderStage::Fragment)

CEpilepsieShaderVS::CEpilepsieShaderVS(CShaderClass* InClass) : CBasicShader(InClass) {}
CEpilepsieShaderFS::CEpilepsieShaderFS(CShaderClass* InClass) : CBasicShader(InClass) {}