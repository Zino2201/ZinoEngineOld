#pragma once

#include "EngineCore.h"

namespace ze
{

enum class NativeWindowFlagBits
{
    Resizable = 1 << 0,
    Maximized = 1 << 1,
    Borderless = 1 << 2,
    Centered = 1 << 3,
    NoBackground = 1 << 4
};
ENABLE_FLAG_ENUMS(NativeWindowFlagBits, NativeWindowFlags);

/**
 * A native window
 */
class ENGINE_API NativeWindow
{
public:
    NativeWindow(const char* in_name, const uint32_t in_width,
        const uint32_t in_height, const int x = 0, const int y = 0, NativeWindowFlags in_flags = NativeWindowFlags());
    ~NativeWindow();

    void set_width(const uint32_t& InWidth); 
    void set_height(const uint32_t& InHeight);
    void* get_handle() const { return handle; }
    const uint32_t& get_width() const { return width; }
    const uint32_t& get_height() const { return height; }
    const char* get_name() const { return name; }
private:
    void* handle;
    const char* name;
    uint32_t width;
    uint32_t height;
};

}