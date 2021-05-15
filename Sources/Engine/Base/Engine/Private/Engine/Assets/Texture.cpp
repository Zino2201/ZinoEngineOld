#include "Engine/Assets/Texture.h"
#include "Threading/JobSystem/Async.h"

namespace ze
{

void Texture::update_resource()
{
	platform_data.load(this);

	using namespace gfx;
	
	TextureCreateInfo tex_create_info;
	tex_create_info.mem_usage = MemoryUsage::GpuOnly;
	tex_create_info.usage_flags = TextureUsageFlagBits::Sampled | TextureUsageFlagBits::TransferDst;
	tex_create_info.array_layers = 1;
	tex_create_info.mip_levels = platform_data.get_mip_count();
	tex_create_info.width = width;
	tex_create_info.height = height;
	tex_create_info.depth = depth;
	tex_create_info.format = platform_data.get_format();

	switch(type)
	{
	case ze::TextureType::Tex1D:
		tex_create_info.type = gfx::TextureType::Tex1D;
		break;

	case ze::TextureType::Tex2D:
		tex_create_info.type = gfx::TextureType::Tex2D;
		break;

	case ze::TextureType::Tex3D:
		tex_create_info.type = gfx::TextureType::Tex3D;
		break;

	case ze::TextureType::Cubemap:
		ZE_CHECK(false);
		break;
	}
		
	auto [result, handle] = Device::get().create_texture(gfx::TextureInfo(tex_create_info));
	texture = handle;

	CommandList* list = Device::get().allocate_cmd_list(CommandListType::Gfx);
	list->texture_barrier(
		*texture,
		PipelineStageFlagBits::TopOfPipe,
		AccessFlags(),
		TextureLayout::Undefined,
		PipelineStageFlagBits::Transfer,
		AccessFlagBits::TransferWrite,
		TextureLayout::TransferDst,
		TextureSubresourceRange(TextureAspectFlagBits::Color, 0, platform_data.get_mip_count(), 0, 1));
	
	/** Transition all subresources to TransferDst */
	for(size_t i = 0; i < platform_data.get_mip_count(); ++i)
	{
		auto& data = platform_data.get_mip(i).data;

		/** Copy texture data to gpu using a staging buffer */
		UniqueBuffer staging_buf(Device::get().create_buffer(BufferInfo::make_staging_buffer(
			data.size()), { (uint8_t*) data.data(), data.size() }).second);
			
		list->copy_buffer_to_texture(staging_buf.get(),
			*texture,
			BufferTextureCopyRegion(0,
				TextureSubresourceLayers(TextureAspectFlagBits::Color, i, 0, 1),
				Offset3D(),
				Extent3D(platform_data.get_mipmaps()[i].width, platform_data.get_mipmaps()[i].height, platform_data.get_mipmaps()[i].depth)));
	}
			
	/** Transfer all to ShaderReadOnly */
	list->texture_barrier(
		*texture,
		PipelineStageFlagBits::Transfer,
		AccessFlagBits::TransferWrite,
		TextureLayout::TransferDst,
		PipelineStageFlagBits::FragmentShader,
		AccessFlagBits::ShaderRead,
		TextureLayout::ShaderReadOnly,
		TextureSubresourceRange(TextureAspectFlagBits::Color, 0, platform_data.get_mip_count(), 0, 1));
	
	Device::get().submit(list);

	TextureViewInfo view_info;
	switch(type)
	{
	case ze::TextureType::Tex1D:
		ZE_DEBUGBREAK();
		break;
	case ze::TextureType::Tex2D:
		view_info = TextureViewInfo::make_2d_view(*texture, platform_data.get_format(), TextureSubresourceRange(
			TextureAspectFlagBits::Color, 0, platform_data.get_mip_count(), 0, 1));
		break;
	case ze::TextureType::Tex3D:
		ZE_DEBUGBREAK();
		break;
	}

	texture_view = Device::get().create_texture_view(view_info).second;

#if !ZE_WITH_EDITOR
	/** Free data from the CPU as we don't need it anymore */
	if(!keep_in_ram)
		platform_data.mipmaps.clear();
#endif

	ready = true;
}

}