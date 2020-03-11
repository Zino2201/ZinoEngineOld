#include "ShaderCompiler.h"
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <fstream>
#include "Core/Engine.h"
#include "Render/RenderSystem/RenderSystem.h"
#include "IO/IOUtils.h"
#include <spirv_cross/spirv_glsl.hpp>

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

#include <iostream>
#include <filesystem>

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
		for (auto it = directoryStack.rbegin(); it != directoryStack.rend(); ++it) 
		{
			std::string path = *it + '/' + headerName;
			std::replace(path.begin(), path.end(), '\\', '/');
			std::ifstream file(path, std::ios_base::binary | std::ios_base::ate);
			if (file) 
			{
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
	switch (InStage)
	{
	case EShaderStage::Vertex:
		return EShLangVertex;
	case EShaderStage::Fragment:
		return EShLangFragment;
	default:
		return EShLangCount;
	}
}


std::vector<SShaderParameterMember> ParseStruct(spirv_cross::Compiler* Compiler,
	const spirv_cross::SPIRType& InSpirType)
{
	std::vector<SShaderParameterMember> Members;
	Members.reserve(InSpirType.member_types.size());

	for (unsigned int i = 0; i < InSpirType.member_types.size(); i++)
	{
		auto& Type = Compiler->get_type(InSpirType.member_types[i]);
		std::vector<SShaderParameterMember> StructMembers;
		if (Type.basetype == spirv_cross::SPIRType::Struct)
			StructMembers = ParseStruct(Compiler, Type);

		const std::string& Name = Compiler->get_member_name(InSpirType.self, i);
		uint64_t Size = Compiler->get_declared_struct_member_size(InSpirType, i);
		uint64_t Offset = Compiler->type_struct_member_offset(InSpirType, i);

		Members.push_back({ Name, Size, Offset, StructMembers });
	}

	return Members;
}

SCompiledShaderData CShaderCompiler::CompileShader(EShaderStage InStage,
	const std::string& InPath,
	const std::string& InEntryPoint)
{
	SCompiledShaderData OutData;
	SRenderSystemDetails Details = g_Engine->GetRenderSystem()->GetRenderSystemDetails();

	std::string ShaderData = IOUtils::ReadTextFile(InPath);

	glslang::TShader GlslShader(ToShLanguage(InStage));
	GlslShader.setEntryPoint(InEntryPoint.c_str());

	std::unique_ptr<spirv_cross::Compiler> Compiler;

	LOG(ELogSeverity::Info, "Shader compiler: Compiling shader %s (sync)", InPath.c_str())

	switch(Details.Format)
	{
	case ERenderSystemShaderFormat::SpirV:
	{
		/** Compile GLSL to SPV */
		int DefaultVersion = 100;

		/** Input GLSL, target vulkan */
		GlslShader.setEnvInput(glslang::EShSourceGlsl, ToShLanguage(InStage),
			glslang::EShClientVulkan, DefaultVersion);

		/** Vulkan v1.0 */
		GlslShader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_0);

		/** Spv v1.0 */
		GlslShader.setEnvTarget(glslang::EShTargetSpv,
			glslang::EShTargetSpv_1_0);

		DirStackFileIncluder Includer;
		Includer.pushExternalLocalDirectory("Assets/Shaders");
		for (auto& p : std::filesystem::recursive_directory_iterator("Assets/Shaders"))
		{
			if (!p.is_directory())
				continue;

			Includer.pushExternalLocalDirectory(p.path().string());
		}

		TBuiltInResource Resources = DefaultTBuiltInResource;
		EShMessages Messages = EShMsgDefault;

		std::string Defines = "#version 450\n#extension GL_GOOGLE_include_directive : enable\n#extension GL_ARB_gpu_shader_int64: require\n";

		switch (InStage)
		{
		case EShaderStage::Vertex:
			Defines += "#define VERTEX_SHADER\n";
			break;
		case EShaderStage::Fragment:
			Defines += "#define FRAGMENT_SHADER\n";
			break;
		}

		/** Preprocess shader */
		std::string PreprocessedGLSL;
		std::string GLSLToPreprocessor = Defines + ShaderData;
		const char* ShaderStr = GLSLToPreprocessor.c_str();

		GlslShader.setStrings(&ShaderStr, 1);

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
		glslang::GlslangToSpv(*Program.getIntermediate(ToShLanguage(InStage)),
			Spv, &Logger, &Options);

		Compiler = std::make_unique<spirv_cross::Compiler>(Spv.data(),
			Spv.size());

		OutData.OutSpv = Spv;

		break;
	}
	default:
		LOG(ELogSeverity::Error, "Unsupported shader format!")
		break;
	}

	/** Parse reflection data */
	spirv_cross::ShaderResources Resources = Compiler->get_shader_resources();

	std::vector<SShaderParameter> Parameters;
	Parameters.reserve(Resources.uniform_buffers.size());

	/** Parse uniform buffers */
	for (auto& UniformBuffer : Resources.uniform_buffers)
	{
		const auto& Type = Compiler->get_type(UniformBuffer.base_type_id);

		unsigned Set = Compiler->get_decoration(UniformBuffer.id, spv::DecorationDescriptorSet);
		unsigned Binding = Compiler->get_decoration(UniformBuffer.id, spv::DecorationBinding);
		std::string Name = Compiler->get_name(UniformBuffer.id);
		if(Name.empty())
			Name = Compiler->get_name(UniformBuffer.base_type_id);

		uint64_t Size = Compiler->get_declared_struct_size(Type);

		std::vector<SShaderParameterMember> Members =
			ParseStruct(Compiler.get(), Type);

		Parameters.push_back({ Name, 
			EShaderParameterType::UniformBuffer,
			Set,
			Binding,
			Size,
			1,
			InStage,
			Members });
	}

	/** Parse combined image sampelrs */
	for(const auto& ImageSampler : Resources.sampled_images)
	{
		const auto& Type = Compiler->get_type(ImageSampler.base_type_id);

		unsigned Set = Compiler->get_decoration(ImageSampler.id, spv::DecorationDescriptorSet);
		unsigned Binding = Compiler->get_decoration(ImageSampler.id, spv::DecorationBinding);
		std::string Name = Compiler->get_name(ImageSampler.id);
		if (Name.empty())
			Name = Compiler->get_name(ImageSampler.base_type_id);

		uint64_t Size = 0;

		Parameters.push_back({ Name,
			EShaderParameterType::CombinedImageSampler,
			Set,
			Binding,
			Size,
			1,
			InStage,
			{} });
	}

	/** Parse storage buffers */
	for (auto& StorageBuffer : Resources.storage_buffers)
	{
		const auto& Type = Compiler->get_type(StorageBuffer.base_type_id);

		unsigned Set = Compiler->get_decoration(StorageBuffer.id, spv::DecorationDescriptorSet);
		unsigned Binding = Compiler->get_decoration(StorageBuffer.id, spv::DecorationBinding);
		std::string Name = Compiler->get_name(StorageBuffer.id);
		if (Name.empty())
			Name = Compiler->get_name(StorageBuffer.base_type_id);

		uint64_t Size = Compiler->get_declared_struct_size(Type);

		std::vector<SShaderParameterMember> Members =
			ParseStruct(Compiler.get(), Type);

		Parameters.push_back({ Name,
			EShaderParameterType::StorageBuffer,
			Set,
			Binding,
			Size,
			1,
			InStage,
			Members });
	}

	OutData.Parameters = Parameters;

	return OutData;
}