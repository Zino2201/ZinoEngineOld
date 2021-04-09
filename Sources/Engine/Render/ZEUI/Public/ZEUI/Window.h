#pragma once

#include "CompositeWidget.h"

struct SDL_Window;

namespace ze::ui
{

class DrawContext;

enum class WindowFlagBits
{
	Centered = 1 << 0,
	Maximized = 1 << 1,
	Resizable = 1 << 2,
};
ENABLE_FLAG_ENUMS(WindowFlagBits, WindowFlags);
	
/**
 * A top-level window
 */
class ZEUI_API Window final : public CompositeWidget
{
public:
	Window(const uint32_t in_width,
		const uint32_t in_height,
		const std::string_view& in_title,
		const uint32_t in_x,
		const uint32_t in_y,
		const WindowFlags in_flags);
	~Window() override;

	void set_width(const uint32_t in_width) { width = in_width; }
	void set_height(const uint32_t in_height) { height = in_height; }
	
	ZE_FORCEINLINE const uint32_t get_width() const { return width; }
	ZE_FORCEINLINE const uint32_t get_height() const { return height; }
	ZE_FORCEINLINE const uint32_t get_x() const { return x; }
	ZE_FORCEINLINE const uint32_t get_y() const { return y; }
	ZE_FORCEINLINE const std::string& get_title() const { return title; }
	ZE_FORCEINLINE SDL_Window* get_handle() const { return handle; }
	
	void compute_desired_size(maths::Vector2f in_available_size) override;
	void arrange_children() override;
	void paint_window(Renderer& renderer);
private:
	uint32_t width;
	uint32_t height;
	std::string title;
	uint32_t x;
	uint32_t y;
	SDL_Window* handle;
};
	
}