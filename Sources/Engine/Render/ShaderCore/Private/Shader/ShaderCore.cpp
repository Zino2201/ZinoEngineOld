#include "Shader/ShaderCore.h"

namespace ZE
{

DEFINE_MODULE(CDefaultModule, ShaderCore);

SShaderParameter::SShaderParameter(const std::string& InName,
	const EShaderParameterType& InType,
	const uint32_t& InSet,
	const uint32_t& InBinding,
	const uint32_t& InCount) :
	Name(InName), Type(InType), Set(InSet), Binding(InBinding), Size(0), Count(InCount) {}

SShaderParameter::SShaderParameter(const std::string& InName,
	const EShaderParameterType& InType,
	const uint32_t& InSet,
	const uint32_t& InBinding,
	const uint64_t& InSize,
	const uint32_t& InCount,
	const std::vector<SShaderParameterMember>& InMembers) :
	Name(InName), Type(InType), Set(InSet), Binding(InBinding), Size(InSize), Count(InCount),
	Members(InMembers) {}

}