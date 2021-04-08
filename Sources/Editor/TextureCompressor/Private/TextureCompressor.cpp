#include "TextureCompressor.h"
#include "nvtt.h"
#include "Threading/JobSystem/Async.h"

namespace ze
{

struct TexcOutputHandler : public nvtt::OutputHandler
{
	TexcOutputHandler(uint8_t* in_target_data) : target_data(in_target_data) {}
	
	void beginImage(int size, int width, int height, int depth, int face, int mipLevel) override {}
	void endImage() override {}

	bool writeData(const void* data, int size) override
	{
		memcpy(target_data, data, size);
		target_data += size;
		return true;
	}
	
	uint8_t* target_data;
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

std::vector<uint8_t> texc_compress(const uint32_t in_width,
	const uint32_t in_height,
	const uint32_t in_depth,
	const std::vector<uint8_t>& in_uncompressed_data,
	const TexcPixelFormat in_src_pixel_format,
	const gfx::Format& in_target_format)
{
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

	/** Calculate final size */
	uint32_t block_width = 4;
	uint32_t block_height = 4;
	uint32_t block_size = (nvtt_format == nvtt::Format_BC1 || nvtt_format == nvtt::Format_BC4) ? 8 : 16;
	uint32_t texture_blocks_width = std::max(in_width / block_width, 1Ui32);
	uint32_t texture_blocks_height = std::max(in_height / block_height, 1Ui32);

	std::vector<uint8_t> output;
	output.resize(texture_blocks_width * texture_blocks_height * block_size);

	/** Calculate jobs count */
	uint32_t blocks_per_job = std::max<uint32_t>(texture_blocks_width, std::ceil(std::log2(2048)));
	uint32_t rows_per_job = blocks_per_job / texture_blocks_width;
	uint32_t job_count = texture_blocks_height / rows_per_job;

	nvtt::OutputOptions output_options;	

	std::vector<std::reference_wrapper<const jobsystem::Job>> jobs;
	jobs.reserve(job_count);

	const uint8_t* src_data_ptr = reinterpret_cast<const uint8_t*>(src_data.data());
	uint8_t* dst_data_ptr = output.data();
	for(uint32_t i = 0; i < job_count; ++i)
	{
		uint32_t height = rows_per_job * block_height;

		jobs.push_back(jobsystem::async(
			[nvtt_format, src_data_ptr, dst_data_ptr, in_width, height](const jobsystem::Job& in_job)
			{
				nvtt::CompressionOptions compression_options;
				compression_options.setFormat(nvtt_format);
				compression_options.setPixelType(nvtt::PixelType::PixelType_UnsignedNorm);
				compression_options.setQuality(nvtt::Quality::Quality_Fastest);
				compression_options.setColorWeights(1, 1, 1);

				nvtt::InputOptions input_opts;
				input_opts.setTextureLayout(nvtt::TextureType_2D, in_width, height);
				input_opts.setMipmapGeneration(false, -1);
				input_opts.setFormat(nvtt::InputFormat_BGRA_8UB);
				input_opts.setMipmapData(src_data_ptr, in_width, height);

				nvtt::OutputOptions output_opts;
				TexcOutputHandler handler(dst_data_ptr);
				TexcErrorHandler error;
				output_opts.setOutputHeader(false);
				output_opts.setOutputHandler(&handler);
				output_opts.setErrorHandler(&error);

				nvtt::Compressor compressor;
				compressor.enableCudaAcceleration(true);
				compressor.process(input_opts, compression_options, output_opts);
			}));

		src_data_ptr += static_cast<uint64_t>(rows_per_job) * block_height * in_width * sizeof(TexcColorRGBA32Unorm);
		dst_data_ptr += rows_per_job * texture_blocks_width * block_size;
	}

	for(const auto& job : jobs)
		jobsystem::wait(job);

	return output;
}

}