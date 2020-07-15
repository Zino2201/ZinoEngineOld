#pragma once

#include "EngineCore.h"

namespace ZE
{

enum class EWindowFlags
{
    None = 1 << 0,

    Resizable = 1 << 1,
    Maximized = 1 << 2,
    Borderless = 1 << 3,
};
DECLARE_FLAG_ENUM(EWindowFlags);

/**
 * A window
 */
class RENDERCORE_API CWindow
{
public:
    CWindow(const char* InName, const uint32_t& InWidth,
        const uint32_t& InHeight, const EWindowFlags& InFlags = EWindowFlags::None);
    ~CWindow();

    void SetWidth(const uint32_t& InWidth) { Width = InWidth; }
    void SetHeight(const uint32_t& InHeight) { Height = InHeight; }
    void* GetHandle() const { return Handle; }
    const uint32_t& GetWidth() const { return Width; }
    const uint32_t& GetHeight() const { return Height; }
private:
    void* Handle;
    const char* Name;
    uint32_t Width;
    uint32_t Height;
};

}