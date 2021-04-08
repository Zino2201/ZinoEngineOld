#include "TextureCompressor.h"
#include "nvtt.h"
#include "Threading/JobSystem/Async.h"

namespace ze
{

struct TexcOutputHandler : public nvtt::OutputHandler
{
	TexcOutputHandler(std::vector<std::vector<uint8_t>>& in_output) : output(in_output), current_mip(nullptr) {}
	
	void beginImage(int size, int width, int height, int depth, int face, int mipLevel) override 
	{
		output[mipLevel].resize(size);
		current_mip = output[mipLevel].data();
	}

	void endImage() override 
	{
		current_mip = nullptr;
	}

	bool writeData(const void* data, int size) override
	{
		ZE_CHECK(current_mip);
		memcpy(current_mip, data, size);
		current_mip += size;
		return true;
	}
	
	std::vector<std::vector<uint8_t>>& output;
	uint8_t* current_mip;
};

struct TexcErrorHandler : public nvtt::ErrorHandler
{
	void error(nvtt::Error error) override
	{
		ze::logger::error("Texture compression error: {}", nvtt::errorString(error));
	}
};

struct TexcColorRGBA32Unorm
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct TexcColorRGB32Unorm
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

std::vector<std::vector<uint8_t>> texc_compress(const uint32_t in_width,
	const uint32_t in_height,
	const uint32_t in_depth,
	const std::vector<uint8_t>& in_uncompressed_data,
	const TexcPixelFormat in_src_pixel_format,
	const gfx::Format& in_target_format,
	const uint32_t in_mip_levels)
{
	ZE_CHECK(in_mip_levels > 0);

	std::vector<TexcColorRGBA32Unorm> src_data;
	src_data.reserve(in_width * in_height * in_depth * 4);

	if(in_src_pixel_format == TexcPixelFormat::RGBA32Unorm)
	{
		/** Flip B & R channels */
		for(size_t i = 0; i < in_uncompressed_data.size(); i += sizeof(TexcColorRGBA32Unorm))
		{
			const TexcColorRGBA32Unorm* color = 
				reinterpret_cast<const TexcColorRGBA32Unorm*>(in_uncompressed_data.data() + i);	
			
			src_data.push_back({ 
				color->b,
				color->g,
				color->r,
				color->a
				});
		}
	}
	else if(in_src_pixel_format == TexcPixelFormat::RGB32Unorm)
	{
		/** Flip B & R channels and add a alpha channel */
		for(size_t i = 0; i < in_uncompressed_data.size(); i += sizeof(TexcColorRGB32Unorm))
		{
			const TexcColorRGB32Unorm* src_color = 
				reinterpret_cast<const TexcColorRGB32Unorm*>(
					in_uncompressed_data.data() + i);	

			src_data.push_back({ 
				src_color->b,
				src_color->g,
				src_color->r,
				255
				});
		}
	}

	nvtt::Format nvtt_format = nvtt::Format::Format_RGBA;
	switch(in_target_format)
	{
	case gfx::Format::R8G8B8A8Unorm:
	case gfx::Format::R8G8B8A8Srgb:
		nvtt_format = nvtt::Format_RGBA;
		break;
	case gfx::Format::Bc1RgbUnormBlock:
	case gfx::Format::Bc1RgbSrgbBlock:
		nvtt_format = nvtt::Format_BC1;
		break;
	case gfx::Format::Bc1RgbaUnormBlock:
	case gfx::Format::Bc1RgbaSrgbBlock:
		nvtt_format = nvtt::Format_BC1a;
		break;
	case gfx::Format::Bc3UnormBlock:
	case gfx::Format::Bc3SrgbBlock:
		nvtt_format = nvtt::Format_BC3;
		break;
	case gfx::Format::Bc5UnormBlock:
	case gfx::Format::Bc5SnormBlock:
		nvtt_format = nvtt::Format_BC5;
		break;
	case gfx::Format::Bc6HUfloatBlock:
	case gfx::Format::Bc6HSfloatBlock:
		nvtt_format = nvtt::Format_BC6;
		break;
	case gfx::Format::Bc7UnormBlock:
	case gfx::Format::Bc7SrgbBlock:
		nvtt_format = nvtt::Format_BC7;
		break;
	default:
		ZE_CHECK(false);
		ze::logger::error("Unsupported format given to texc_compress");
		return {};
	}

	std::vector<std::vector<uint8_t>> output;
	output.resize(in_mip_levels);

	nvtt::OutputOptions output_options;	
	nvtt::CompressionOptions compression_options;
	compression_options.setFormat(nvtt_format);
	compression_options.setPixelType(nvtt::PixelType::PixelType_UnsignedNorm);
	compression_options.setQuality(nvtt::Quality::Quality_Fastest);
	compression_options.setColorWeights(1, 1, 1);

	nvtt::InputOptions input_opts;
	input_opts.setTextureLayout(nvtt::TextureType_2D, in_width, in_height);
	input_opts.setMipmapGeneration(false, -1);
	input_opts.setFormat(nvtt::InputFormat_BGRA_8UB);
	input_opts.setMipmapData(src_data.data(), in_width, in_height, 1, 0, 0);
	input_opts.setMipmapGeneration(in_mip_levels > 1, in_mip_levels);

	nvtt::OutputOptions output_opts;
	TexcOutputHandler handler(output);
	TexcErrorHandler error;
	output_opts.setOutputHeader(false);
	output_opts.setOutputHandler(&handler);
	output_opts.setErrorHandler(&error);

	nvtt::Compressor compressor;
	compressor.enableCudaAcceleration(true);
	compressor.process(input_opts, compression_options, output_opts);

	/** Flip B & R channels if required */
	if(nvtt_format == nvtt::Format_RGBA)
	{
		for(size_t mip = 0; mip < in_mip_levels; ++mip)
		{
			for(size_t i = 0; i < output[mip].size(); i += sizeof(TexcColorRGBA32Unorm))
			{
				TexcColorRGBA32Unorm color = *reinterpret_cast<const TexcColorRGBA32Unorm*>(output[mip].data() + i);	
			
				output[mip][i + 0] = color.b;
				output[mip][i + 1] = color.g;
				output[mip][i + 2] = color.r;
				output[mip][i + 3] = color.a;
			}
		}
	}

	return output;
}

}