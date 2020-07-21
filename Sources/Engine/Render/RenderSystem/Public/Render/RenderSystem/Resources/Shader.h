#pragma once

#include "Resource.h"
#include "Shader/ShaderCore.h"

namespace ZE
{

struct SRSShaderCreateInfo
{
    EShaderStage Stage;
    uint64_t BytecodeSize;
    const void* Bytecode;
    SShaderParameterMap ParameterMap;

	SRSShaderCreateInfo(const EShaderStage& InStage,
		const uint64_t& InBytecodeSize,
		const void* InBytecode,
		const SShaderParameterMap& InParameterMap) :
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

    FORCEINLINE const SRSShaderCreateInfo& GetCreateInfo() const { return CreateInfo; }
protected:
    SRSShaderCreateInfo CreateInfo;
};

}