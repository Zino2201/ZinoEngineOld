#pragma once

#include "EngineCore.h"

namespace ZE
{

enum class EWindowFlagBits
{
    None = 0,

    Resizable = 1 << 0,
    Maximized = 1 << 1,
    Borderless = 1 << 2,
    Centered = 1 << 3,
};
ENABLE_FLAG_ENUMS(EWindowFlagBits, EWindowFlags);

/**
 * A window
 */
class RENDERCORE_API CWindow
{
public:
    CWindow(const char* InName, const uint32_t& InWidth,
        const uint32_t& InHeight, const EWindowFlags& InFlags = EWindowFlagBits::None);
    ~CWindow();

    void SetWidth(const uint32_t& InWidth); 
    void SetHeight(const uint32_t& InHeight);
    void* GetHandle() const { return Handle; }
    const uint32_t& GetWidth() const { return Width; }
    const uint32_t& GetHeight() const { return Height; }
    const char* GetName() const { return Name; }
private:
    void* Handle;
    const char* Name;
    uint32_t Width;
    uint32_t Height;
};

}