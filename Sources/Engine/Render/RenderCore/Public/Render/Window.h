#pragma once

#include "EngineCore.h"

namespace ZE
{

/**
 * A window
 */
class RENDERCORE_API CWindow
{
public:
    CWindow(const char* InName, const uint32_t& InWidth,
        const uint32_t& InHeight);
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