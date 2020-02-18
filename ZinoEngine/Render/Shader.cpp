#include "Shader.h"
#include "Core/Engine.h"
#include "RenderSystem/RenderSystem.h"
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <fstream>
#include <spirv_cross/spirv_glsl.hpp>
#include <magic_enum.hpp>
#include "IO/IOUtils.h"

const TLimits DefaultLimits = {
	/* .nonInductiveForLoops = */ 1,
	/* .whileLoops = */ 1,
	/* .doWhileLoops = */ 1,
	/* .generalUniformIndexing = */ 1,
	/* .generalAttributeMatrixVectorIndexing = */ 1,
	/* .generalVaryingIndexing = */ 1,
	/* .generalSamplerIndexing = */ 1,
	/* .generalVariableIndexing = */ 1,
	/* .generalConstantMatrixVectorIndexing = */ 1,
};

const TBuiltInResource DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* .limits = */ DefaultLimits,
};

/**
 * Shader class
 */
std::map<std::string, CShaderClass*> CShaderClass::ShaderClasses;

CShaderClass::CShaderClass(const std::string& InName, const std::string& InFilename,
	EShaderStage InStage,
	InstantiateFunctionType InInstantiateFunction) 
	: Name(InName), Filename(InFilename), Stage(InStage), InstantiateFunction(InInstantiateFunction)
{
	ShaderClasses.insert(std::make_pair(Name, this));
}

void CShaderClass::AddShader(CShader* InShader)
{
	Shaders.push_back(InShader);
}

void CShaderClass::ClearShaders()
{
	Shaders.clear();
}

CShader* CShaderClass::InstantiateShader()
{
	CShader* Shader = InstantiateFunction(this);
	AddShader(Shader);
	return Shader;
}

// Default include class for normal include convention of search backward
// through the stack of active include paths (for nested includes).
// Can be overridden to customize.
class DirStackFileIncluder : public glslang::TShader::Includer {
public:
	DirStackFileIncluder() : externalLocalDirectoryCount(0) { }

	virtual IncludeResult* includeLocal(const char* headerName,
		const char* includerName,
		size_t inclusionDepth) override
	{
		return readLocalPath(headerName, includerName, (int)inclusionDepth);
	}

	virtual IncludeResult* includeSystem(const char* headerName,
		const char* /*includerName*/,
		size_t /*inclusionDepth*/) override
	{
		return readSystemPath(headerName);
	}

	// Externally set directories. E.g., from a command-line -I<dir>.
	//  - Most-recently pushed are checked first.
	//  - All these are checked after the parse-time stack of local directories
	//    is checked.
	//  - This only applies to the "local" form of #include.
	//  - Makes its own copy of the path.
	virtual void pushExternalLocalDirectory(const std::string& dir)
	{
		directoryStack.push_back(dir);
		externalLocalDirectoryCount = (int)directoryStack.size();
	}

	virtual void releaseInclude(IncludeResult* result) override
	{
		if (result != nullptr) {
			delete[] static_cast<tUserDataElement*>(result->userData);
			delete result;
		}
	}

	virtual ~DirStackFileIncluder() override { }

protected:
	typedef char tUserDataElement;
	std::vector<std::string> directoryStack;
	int externalLocalDirectoryCount;

	// Search for a valid "local" path based on combining the stack of include
	// directories and the nominal name of the header.
	virtual IncludeResult* readLocalPath(const char* headerName, const char* includerName, int depth)
	{
		// Discard popped include directories, and
		// initialize when at parse-time first level.
		directoryStack.resize(depth + externalLocalDirectoryCount);
		if (depth == 1)
			directoryStack.back() = getDirectory(includerName);

		// Find a directory that works, using a reverse search of the include stack.
		for (auto it = directoryStack.rbegin(); it != directoryStack.rend(); ++it) {
			std::string path = *it + '/' + headerName;
			std::replace(path.begin(), path.end(), '\\', '/');
			std::ifstream file(path, std::ios_base::binary | std::ios_base::ate);
			if (file) {
				directoryStack.push_back(getDirectory(path));
				return newIncludeResult(path, file, (int)file.tellg());
			}
		}

		return nullptr;
	}

	// Search for a valid <system> path.
	// Not implemented yet; returning nullptr signals failure to find.
	virtual IncludeResult* readSystemPath(const char* /*headerName*/) const
	{
		return nullptr;
	}

	// Do actual reading of the file, filling in a new include result.
	virtual IncludeResult* newIncludeResult(const std::string& path, std::ifstream& file, int length) const
	{
		char* content = new tUserDataElement[length];
		file.seekg(0, file.beg);
		file.read(content, length);
		return new IncludeResult(path, content, length, content);
	}

	// If no path markers, return current working directory.
	// Otherwise, strip file name and return path leading up to it.
	virtual std::string getDirectory(const std::string path) const
	{
		size_t last = path.find_last_of("/\\");
		return last == std::string::npos ? "." : path.substr(0, last);
	}
};

EShLanguage ToShLanguage(EShaderStage InStage)
{
	switch(InStage)
	{
	case EShaderStage::Vertex:
		return EShLangVertex;
	case EShaderStage::Fragment:
		return EShLangFragment;
	default:
		return EShLangCount;
	}
}

CShader::CShader(CShaderClass* InClass) : ShaderClass(InClass)
{
	SRenderSystemDetails Details = g_Engine->GetRenderSystem()->GetRenderSystemDetails();
	
	std::string ShaderData = IOUtils::ReadTextFile(InClass->GetFilename());
	const char* ShaderStr = ShaderData.c_str();

	glslang::TShader GlslShader(ToShLanguage(ShaderClass->GetStage()));
	GlslShader.setStrings(&ShaderStr, 1);
	GlslShader.setEntryPoint("main");

	std::unique_ptr<spirv_cross::Compiler> Compiler;

	LOG(ELogSeverity::Info, "Compiling shader %s", InClass->GetFilename().c_str())

	switch(Details.Format)
	{
	case ERenderSystemShaderFormat::SpirV:
	{
		/** Compile GLSL to SPV */

		int DefaultVersion = 100;

		/** Input GLSL, target vulkan */
		GlslShader.setEnvInput(glslang::EShSourceGlsl, ToShLanguage(InClass->GetStage()),
			glslang::EShClientVulkan, DefaultVersion);

		/** Vulkan v1.0 */
		GlslShader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);

		/** Spv v1.0 */
		GlslShader.setEnvTarget(glslang::EShTargetSpv,
			glslang::EShTargetSpv_1_0);

		DirStackFileIncluder Includer;
		TBuiltInResource Resources = DefaultTBuiltInResource;
		EShMessages Messages = EShMsgDefault;

		/** Preprocess shader */
		std::string PreprocessedGLSL;

		if(!GlslShader.preprocess(&Resources, DefaultVersion, ENoProfile, false,
			false, Messages, &PreprocessedGLSL, Includer))
		{
			LOG(ELogSeverity::Fatal, "Failed to preprocess GLSL: %s\nDebug Log: %s",
				GlslShader.getInfoLog(), GlslShader.getInfoDebugLog());
		}

		const char* PreprocessedConst = PreprocessedGLSL.c_str();
		GlslShader.setStrings(&PreprocessedConst, 1);
		if(!GlslShader.parse(&Resources, DefaultVersion, false, Messages, Includer))
		{
			LOG(ELogSeverity::Fatal, "Failed to parse GLSL: %s\nDebug Log: %s",
				GlslShader.getInfoLog(), GlslShader.getInfoDebugLog());
		}

		/** Create program and link it */
		glslang::TProgram Program;
		Program.addShader(&GlslShader);
		if(!Program.link(Messages))
		{
			LOG(ELogSeverity::Fatal, "Failed to link program: %s\nDebug Log: %s",
				GlslShader.getInfoLog(), GlslShader.getInfoDebugLog());
		}

		std::vector<unsigned int> Spv;
		spv::SpvBuildLogger Logger;
		glslang::SpvOptions Options;
		glslang::GlslangToSpv(*Program.getIntermediate(ToShLanguage(InClass->GetStage())),
			Spv, &Logger, &Options);
		
		Shader = g_Engine->GetRenderSystem()->CreateShader(Spv.data(), 
			Spv.size() * 4, ShaderClass->GetStage());

		Compiler = std::make_unique<spirv_cross::Compiler>(Spv.data(),
			Spv.size());

		break;
	}
	default:
		LOG(ELogSeverity::Error, "Unsupported shader format!")
		break;
	}

	/** Parse reflection data */
	spirv_cross::ShaderResources Resources = Compiler->get_shader_resources();
	
	/** Parse uniform buffers */
	for (auto& UniformBuffer : Resources.uniform_buffers)
	{
		unsigned Set = Compiler->get_decoration(UniformBuffer.id, spv::DecorationDescriptorSet);
		unsigned Binding = Compiler->get_decoration(UniformBuffer.id, spv::DecorationBinding);
		auto Type = magic_enum::enum_name<spirv_cross::SPIRType::BaseType>(
			Compiler->get_type(UniformBuffer.type_id).basetype);
		std::string Name = Compiler->get_name(UniformBuffer.id);
		
		LOG(ELogSeverity::Debug, "UBO %s, %s: Set %d, Binding %d",
			Name.c_str(), Type.data(), Set, Binding);
	}

	/** Input */
	for(auto& Input : Resources.stage_inputs)
	{
		unsigned Set = Compiler->get_decoration(Input.id, spv::DecorationDescriptorSet);
		unsigned Binding = Compiler->get_decoration(Input.id, spv::DecorationBinding);
		auto Type = magic_enum::enum_name<spirv_cross::SPIRType::BaseType>(
			Compiler->get_type(Input.type_id).basetype);
		std::string Name = Compiler->get_name(Input.id);

		LOG(ELogSeverity::Debug, "Input %s, %s: Set %d, Binding %d",
			Name.c_str(), Type.data(), Set, Binding);
	}
}

CShader::~CShader() {}