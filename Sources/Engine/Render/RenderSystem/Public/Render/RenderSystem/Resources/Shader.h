#pragma once

#include "Resource.h"
#include "Shader/ShaderCore.h"

namespace ze::gfx::shaders
{

struct SRSShaderCreateInfo
{
    ShaderStage Stage;
    uint64_t BytecodeSize;
    const void* Bytecode;
    ShaderParameterMap ParameterMap;

	SRSShaderCreateInfo(const ShaderStage& InStage,
		const uint64_t& InBytecodeSize,
		const void* InBytecode,
		const ShaderParameterMap& InParameterMap) :
	    Stage(InStage), BytecodeSize(InBytecodeSize), Bytecode(InBytecode), 
        ParameterMap(InParameterMap) {}
};

/**
 * A handle to a shader
 */
class CRSShader : public CRSResource
{
public:
    CRSShader(const SRSShaderCreateInfo& InCreateInfo) :
        CreateInfo(InCreateInfo) {}

    ZE_FORCEINLINE const SRSShaderCreateInfo& GetCreateInfo() const { return CreateInfo; }
protected:
    SRSShaderCreateInfo CreateInfo;
};

}